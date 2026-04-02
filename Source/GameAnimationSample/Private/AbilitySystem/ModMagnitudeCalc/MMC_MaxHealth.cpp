// Copyright Daisy


#include "AbilitySystem/ModMagnitudeCalc/MMC_MaxHealth.h"

#include "AbilitySystem/TaFeiAttributeSet.h"
#include "Interaction/TaFeiCombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	// 捕获 Target (受击者/拥有者) 的 Vigor 属性
	VigorDef.AttributeToCapture = UTaFeiAttributeSet::GetVigorAttribute(); // 注意：请确保属性集里已添加 Vigor
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	VigorDef.bSnapshot = false; // 不打快照，应用时实时计算
    
	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// 收集 Source 和 Target 的 Tags
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
    
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// 获取捕获到的 Vigor 属性值
	float Vigor = 0.f;
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluationParameters, Vigor);
	Vigor = FMath::Max<float>(Vigor, 0.f);

	// 获取玩家/敌人等级 (通过你的 TaFei 战斗接口)
	int32 PlayerLevel = 1;
	if (Spec.GetContext().GetSourceObject()->Implements<UTaFeiCombatInterface>())
	{
		PlayerLevel = ITaFeiCombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}
    
	// 基础 80 + 活力收益 + 等级收益
	return 80.f + 2.5f * Vigor + 10.f * PlayerLevel;
}