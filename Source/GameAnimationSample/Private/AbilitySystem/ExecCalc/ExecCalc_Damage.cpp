// Copyright Daisy


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystem/TaFeiAttributeSet.h"
#include "TaFeiGameplayTags.h"
//  定义一个用于捕获属性的结构体
struct TaFeiDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageReduction);

	TaFeiDamageStatics()
	{
		// 捕获攻击者 (Source) 的伤害增幅倍率
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTaFeiAttributeSet, DamageMultiplier, Source, false);
		// 捕获受击者 (Target) 的减伤比例
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTaFeiAttributeSet, DamageReduction, Target, false);
	}
};

// 静态获取该结构体的函数
static const TaFeiDamageStatics& DamageStatics()
{
	static TaFeiDamageStatics Statics;
	return Statics;
}

//  构造函数：告诉引擎我们要捕获哪些属性
UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().DamageMultiplierDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageReductionDef);
}

// 核心计算逻辑
void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionModifiers) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// ----------获取基础伤害 ----------
	// 从 SetByCaller 中获取我们在 GA 里打包好的物理伤害数值
	float BaseDamage = Spec.GetSetByCallerMagnitude(FTaFeiGameplayTags::Get().Damage_Physical, false, 0.f);

	// ---------- 获取捕获的属性 ----------
	float SourceDamageMultiplier = 1.f; // 默认 1 倍
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageMultiplierDef, EvaluationParameters, SourceDamageMultiplier);
	SourceDamageMultiplier = FMath::Max<float>(SourceDamageMultiplier, 0.f);

	float TargetDamageReduction = 0.f;  // 默认 0 减伤
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageReductionDef, EvaluationParameters, TargetDamageReduction);
	TargetDamageReduction = FMath::Clamp<float>(TargetDamageReduction, 0.f, 1.f); // 确保减伤在 0~1 之间 (例如 0.9 代表减伤 90%)

	// ---------- 计算最终伤害 ----------
	// 公式：最终伤害 = 基础伤害 * 攻击者伤害增幅 * (1 - 受击者减伤比例)
	float FinalDamage = BaseDamage * SourceDamageMultiplier * (1.f - TargetDamageReduction);

	// ----------  输出结果到 IncomingDamage ----------
	if (FinalDamage > 0.f)
	{
		// 将计算好的最终伤害，以 Additive (加法) 的形式修改给 IncomingDamage
		OutExecutionModifiers.AddOutputModifier(FGameplayModifierEvaluatedData(UTaFeiAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, FinalDamage));
	}
}