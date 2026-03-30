// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "TaFeiAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API UTaFeiAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	UTaFeiAbilitySystemComponent();
	// 自定义的 Tag 输入处理函数
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
};
