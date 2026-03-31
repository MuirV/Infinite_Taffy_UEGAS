// Copyright Daisy


#include "AbilitySystem/TaFeiAbilitySystemComponent.h"

UTaFeiAbilitySystemComponent::UTaFeiAbilitySystemComponent()
{

	// 后续如果需要绑定特殊的输入逻辑，可以在这里加，现在空着
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