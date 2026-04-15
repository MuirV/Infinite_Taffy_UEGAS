// Copyright Daisy


#include "AbilitySystem/TaFeiAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "GameAnimationSample/GameAnimationSample.h"
#include "Interaction/TaFeiCombatInterface.h"
#include "Interaction/TaFeiPlayerInterface.h"

UTaFeiAbilitySystemComponent::UTaFeiAbilitySystemComponent()
{

	// 后续如果需要绑定特殊的输入逻辑，可以在这里加，现在空着
}

void UTaFeiAbilitySystemComponent::AbilityActorInfoSet()
{
	// 当有 GameplayEffect 应用到自己身上时，触发本类的 ClientEffectApplied 函数
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UTaFeiAbilitySystemComponent::ClientEffectApplied);
	
	// 获取无敌 Tag 
	const FGameplayTag InvincibleTag = FGameplayTag::RequestGameplayTag(FName("State.Invincible"));
	
	// 绑定监听器：只有在 Tag 新增 (0->1) 或移除 (1->0) 时才会触发
	RegisterGameplayTagEvent(InvincibleTag, EGameplayTagEventType::NewOrRemoved).AddUObject(
		this, &UTaFeiAbilitySystemComponent::OnInvincibleTagChanged);
	
}

void UTaFeiAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	
	// 把这个刚刚应用成功的 GE 里的所有 Asset Tags 提取出来
	EffectSpec.GetAllAssetTags(TagContainer);
	
	EffectAssetTags.Broadcast(TagContainer);
}

void UTaFeiAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	// TaFeiPlayerState
	AActor* OwningActor = GetOwnerActor();

	if (OwningActor && OwningActor->Implements<UTaFeiPlayerInterface>())
	{
		// 从PlayerState获取 是否有属性点？
		if (ITaFeiPlayerInterface::Execute_GetAttributePoints(OwningActor) > 0)
		{
			ServerUpdateAttribute(AttributeTag);
		}
	}
}


void UTaFeiAbilitySystemComponent::ServerUpdateAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;


	// 改名：使用 OwningActor 避免和父类成员变量重名  源码底层UAbilitySystemComponent已经定义了OwnerActor，所以命名为OwningActor
	AActor* OwningActor = GetOwnerActor();
	
	// 发送事件触发 GA (通过PS找到 ASC，获得Strength，Vigor等加点) 这里换成OwningActor
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwningActor, AttributeTag, Payload);
	if (OwningActor && OwningActor->Implements<UTaFeiPlayerInterface>())
	{
		// 扣除 1 个属性点！
		ITaFeiPlayerInterface::Execute_AddToAttributePoints(OwningActor, -1);
	}
	
}

// 在 TaFeiAbilitySystemComponent.cpp 末尾补充以下实现：

void UTaFeiAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	// 锁定技能列表，防止在遍历时被修改引发崩溃
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}

FGameplayTag UTaFeiAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		const FGameplayTagContainer& Tags = AbilitySpec.Ability->GetAssetTags(); //API  换用TagContainer

		for (const FGameplayTag& Tag : Tags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UTaFeiAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		// 匹配你项目里的 InputTag 父级标签 (例如 InputTag.LMB)
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

void UTaFeiAbilitySystemComponent::AddStartupAbilitiesFromData(const UCharacterClassInfo* CharacterData, ETaFeiCharacterClass CharacterClass, int32 Level)
{
	if (!CharacterData) return;

	// 防止重复Give（非常关键）
	if (bStartupAbilitiesGiven) return;
	
	// CommonAbilities（所有角色都有） 这里忘记传commonabilities给playerstate，不传会导致commonAbility给不到player身上
	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterData->CommonAbilities)
	{
		if (!AbilityClass) continue;

		FGameplayAbilitySpec Spec(AbilityClass, Level);
		GiveAbility(Spec);
	}
	
	const FTaFeiCharacterClassDefaultInfo& ClassInfo = CharacterData->GetClassDefaultInfo(CharacterClass);

	for (const FTaFeiAbilityInfo& AbilityInfo : ClassInfo.StartupAbilities)
	{
		if (!AbilityInfo.AbilityClass) continue;

		FGameplayAbilitySpec Spec(AbilityInfo.AbilityClass, Level);

		if (AbilityInfo.InputTag.IsValid() && CharacterClass == ETaFeiCharacterClass::Player)
		{
			Spec.GetDynamicSpecSourceTags().AddTag(AbilityInfo.InputTag);
		}

		GiveAbility(Spec);
	}

	
	// 这是在服务器端执行的，如果是局域网主机(Listen Server)玩家，UI 会在这里立刻触发
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast(this);
}

void UTaFeiAbilitySystemComponent::OnInvincibleTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	
	AActor* Avatar = GetAvatarActor();
	
	if (Avatar && Avatar->Implements<UTaFeiCombatInterface>())
	{
		if (NewCount > 0)
		{
			// Tag 数量大于 0，说明刚进入无敌状态，开启描边（传入青色数值 251）
			ITaFeiCombatInterface::Execute_HighlightActor(Avatar, CUSTOM_DEPTH_CYAN);
		}
		else
		{
			// Tag 数量归零，说明无敌时间彻底结束，关闭描边
			ITaFeiCombatInterface::Execute_UnHighlightActor(Avatar);
		}
	}
}


void UTaFeiAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	
	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast(this); // 客户端在这里触发 UI 刷新！
	}
}


void UTaFeiAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	
	for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			
			AbilitySpecInputPressed(AbilitySpec);

			
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UTaFeiAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UTaFeiAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			// 通知技能内部：按键松开了
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}