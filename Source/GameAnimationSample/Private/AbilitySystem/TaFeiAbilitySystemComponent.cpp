// Copyright Daisy


#include "AbilitySystem/TaFeiAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
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