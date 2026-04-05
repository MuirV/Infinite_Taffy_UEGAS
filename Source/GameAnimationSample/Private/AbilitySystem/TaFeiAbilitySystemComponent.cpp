// Copyright Daisy


#include "AbilitySystem/TaFeiAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/TaFeiPlayerInterface.h"

UTaFeiAbilitySystemComponent::UTaFeiAbilitySystemComponent()
{

	// 后续如果需要绑定特殊的输入逻辑，可以在这里加，现在空着
}

void UTaFeiAbilitySystemComponent::AbilityActorInfoSet()
{
	// ★ 当有 GameplayEffect 应用到自己身上时，触发本类的 ClientEffectApplied 函数
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UTaFeiAbilitySystemComponent::ClientEffectApplied);
}

void UTaFeiAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	
	// ★ 核心：把这个刚刚应用成功的 GE 里的所有 Asset Tags 提取出来
	EffectSpec.GetAllAssetTags(TagContainer);
	
	// 把拿到的 Tag 广播出去！(OverlayWidgetController 此时就在乖乖监听这个)
	EffectAssetTags.Broadcast(TagContainer);
}

void UTaFeiAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UTaFeiPlayerInterface>())
	{
		if (ITaFeiPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
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

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

	if (GetAvatarActor()->Implements<UTaFeiPlayerInterface>())
	{
		ITaFeiPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
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

	const FTaFeiCharacterClassDefaultInfo& ClassInfo = CharacterData->GetClassDefaultInfo(CharacterClass);

	for (const FTaFeiAbilityInfo& AbilityInfo : ClassInfo.StartupAbilities)
	{
		if (!AbilityInfo.AbilityClass) continue;

		FGameplayAbilitySpec Spec(AbilityInfo.AbilityClass, Level);

		if (AbilityInfo.InputTag.IsValid())
		{
			Spec.GetDynamicSpecSourceTags().AddTag(AbilityInfo.InputTag);
		}

		GiveAbility(Spec);
	}

	// ★ 核心：在这里统一标记并广播！
	// 这是在服务器端执行的，如果是局域网主机(Listen Server)玩家，UI 会在这里立刻触发
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast(this);
}


void UTaFeiAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	// 客户端接收到技能列表复制时，通知 UI 更新
	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast(this); // 客户端在这里触发 UI 刷新！
	}
}


void UTaFeiAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	// 遍历身上所有已被赋予的技能
	for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		// 检查这个技能是否绑定了这个输入 Tag
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			// 通知技能内部：按键被按下了
			AbilitySpecInputPressed(AbilitySpec);

			// 如果技能还没在运行，就尝试激活它
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