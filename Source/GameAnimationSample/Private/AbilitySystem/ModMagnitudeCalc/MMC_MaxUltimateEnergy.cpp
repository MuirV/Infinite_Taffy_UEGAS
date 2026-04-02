// Copyright Daisy


#include "AbilitySystem/ModMagnitudeCalc/MMC_MaxUltimateEnergy.h"

#include "AbilitySystem/TaFeiAttributeSet.h"
#include "Interaction/TaFeiCombatInterface.h"

UMMC_MaxUltimateEnergy::UMMC_MaxUltimateEnergy()
{
	// 捕获 Target (受击者/拥有者) 的 Intelligence 属性
	// (如果你想用力量 Strength，这里就改成 GetStrengthAttribute())
	IntDef.AttributeToCapture = UTaFeiAttributeSet::GetIntelligenceAttribute(); 
	IntDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	IntDef.bSnapshot = false; 
    
	RelevantAttributesToCapture.Add(IntDef);
}

float UMMC_MaxUltimateEnergy::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
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

	// 获取玩家/敌人等级 (通过你的 TaFei 战斗接口)
	int32 PlayerLevel = 1;
	if (Spec.GetContext().GetSourceObject()->Implements<UTaFeiCombatInterface>())
	{
		PlayerLevel = ITaFeiCombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}
    
	// 大招能量公式设计：
	// 假设基础就是 100，智力收益给极低 (比如每点智力加 0.5 能量上限)，等级收益也给很低 (每级加 2)
	// 这样可以保证大招攒满的时间不会随着等级飙升而彻底失衡。
	return 100.f + 0.5f * Int + 2.0f * PlayerLevel;
}