// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "TaFeiAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API UTaFeiAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
public:
	// 自定义 Context
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
