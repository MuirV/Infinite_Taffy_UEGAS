// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_MaxUltimateEnergy.generated.h"

/**
 * 动态计算最大大招能量 (MaxUltimateEnergy)
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API UMMC_MaxUltimateEnergy : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	UMMC_MaxUltimateEnergy();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	// 我们用智力(Intelligence)来轻微加成大招能量上限
	FGameplayEffectAttributeCaptureDefinition IntDef;
};
