// Copyright Daisy


#include "AbilitySystem/ModMagnitudeCalc/MMC_MaxMana.h"

#include "AbilitySystem/TaFeiAttributeSet.h"
#include "Interaction/TaFeiCombatInterface.h"

UMMC_MaxMana::UMMC_MaxMana()
{
	// 捕获 Target (受击者/拥有者) 的 Intelligence 属性
	IntDef.AttributeToCapture = UTaFeiAttributeSet::GetIntelligenceAttribute(); // 注意：请确保属性集里已添加 Intelligence
	IntDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	IntDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(IntDef);
}

float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// 收集 Source 和 Target 的 Tags
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
    
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// 获取捕获到的 Intelligence 属性值
	float Int = 0.f;
	GetCapturedAttributeMagnitude(IntDef, Spec, EvaluationParameters, Int);
	Int = FMath::Max<float>(Int, 0.f);

	// 获取玩家/敌人等级
	int32 PlayerLevel = 1;
	if (Spec.GetContext().GetSourceObject()->Implements<UTaFeiCombatInterface>())
	{
		PlayerLevel = ITaFeiCombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}
    
	// 基础 50 + 智力收益 + 等级收益
	return 50.f + 2.5f * Int + 15.f * PlayerLevel;
}

