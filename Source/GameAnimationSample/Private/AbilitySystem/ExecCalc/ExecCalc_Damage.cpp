// Copyright Daisy


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystem/TaFeiAttributeSet.h"
#include "AbilitySystem/TaFeiAbilitySystemLibrary.h" 
#include "Interaction/TaFeiCombatInterface.h"

#include "TaFeiGameplayTags.h"

//  定义一个用于捕获属性的结构体
struct FTaFeiDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageReduction);

	// --- Aura 战斗标准二级属性 ---
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	
	FTaFeiDamageStatics()
	{
		// 捕获攻击者 (Source) 的伤害增幅倍率
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTaFeiAttributeSet, DamageMultiplier, Source, false);
		// 捕获受击者 (Target) 的减伤比例
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTaFeiAttributeSet, DamageReduction, Target, false);

		// Aura 标准
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTaFeiAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTaFeiAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTaFeiAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTaFeiAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTaFeiAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTaFeiAttributeSet, CriticalHitDamage, Source, false);
	
	}
};

// 静态获取该结构体的函数
static const FTaFeiDamageStatics& DamageStatics()
{
	static FTaFeiDamageStatics Statics;
	return Statics;
}

//  构造函数：告诉引擎我们要捕获哪些属性
UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().DamageMultiplierDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageReductionDef);

	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
}

// 核心计算逻辑
void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionModifiers) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	int32 SourcePlayerLevel = 1;
	if (SourceAvatar && SourceAvatar->Implements<UTaFeiCombatInterface>())
	{
		SourcePlayerLevel = ITaFeiCombatInterface::Execute_GetPlayerLevel(SourceAvatar);
	}

	int32 TargetPlayerLevel = 1;
	if (TargetAvatar && TargetAvatar->Implements<UTaFeiCombatInterface>())
	{
		TargetPlayerLevel = ITaFeiCombatInterface::Execute_GetPlayerLevel(TargetAvatar);
	}
	// =========================================================================
	// 新增部分结束 (为后续护甲CurveTable、等级压制等复杂公式做好了准备)
	
	// ----------获取基础伤害 ----------
	// 从 SetByCaller 中获取我们在 GA 里打包好的物理伤害数值
	float Damage = Spec.GetSetByCallerMagnitude(FTaFeiGameplayTags::Get().Damage_Physical, false, 0.f);

	// ---------- 获取捕获的属性 ----------
	float SourceDamageMultiplier = 1.f; // 默认 1 倍
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageMultiplierDef, EvaluationParameters, SourceDamageMultiplier);
	SourceDamageMultiplier = FMath::Max<float>(SourceDamageMultiplier, 0.f);

	float TargetDamageReduction = 0.f;  // 默认 0 减伤
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageReductionDef, EvaluationParameters, TargetDamageReduction);
	TargetDamageReduction = FMath::Clamp<float>(TargetDamageReduction, 0.f, 1.f); // 确保减伤在 0~1 之间 (例如 0.9 代表减伤 90%)

	// 应用增减伤与增伤
	// 公式：最终伤害 = 基础伤害 * 攻击者伤害增幅 * (1 - 受击者减伤比例)
	Damage = Damage * SourceDamageMultiplier * (1.f - TargetDamageReduction);

	// Aura那套 额外伤害计算 结算 格挡 (Block)
    // =========================================================================
    float TargetBlockChance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
    TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);

    // 判定是否格挡 (1~100随机数小于格挡率则判定成功)
    const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
    
    
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
    UTaFeiAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
    
    // 流水线加工：如果格挡成功，伤害直接减半
    Damage = bBlocked ? Damage / 2.f : Damage;

    // =========================================================================
    // 【第四步】 结算 护甲 (Armor) 与 破甲 (Armor Penetration) (无曲线表优化版)
    // =========================================================================
    float TargetArmor = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
    TargetArmor = FMath::Max<float>(TargetArmor, 0.f);

    float SourceArmorPenetration = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
    SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.f);

    // 优化公式 1：计算破甲比例 (假设 1点破甲 无视 1%护甲，最高无视 100%)
    float ArmorPenetrationPct = FMath::Clamp(SourceArmorPenetration / 100.f, 0.f, 1.f);
    
    // 扣除破甲后，目标剩下的“有效护甲”
    float EffectiveArmor = TargetArmor * (1.f - ArmorPenetrationPct);
    EffectiveArmor = FMath::Max<float>(EffectiveArmor, 0.f);

    // 优化公式 2：护甲减伤比例 (假设 1点有效护甲 减免 1%伤害)
    // 钳制最高减免 85%，防止伤害变成0或者负数
    float ArmorMitigation = FMath::Clamp(EffectiveArmor, 0.f, 85.f);
    
    // 流水线加工：护甲削弱伤害
    Damage *= (100.f - ArmorMitigation) / 100.f;

    // =========================================================================
    // 【第五步】 结算 暴击 (Critical Hit) (无曲线表优化版)
    // =========================================================================
    float SourceCriticalHitChance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
    SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);

    float TargetCriticalHitResistance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
    TargetCriticalHitResistance = FMath::Max<float>(TargetCriticalHitResistance, 0.f);

    float SourceCriticalHitDamage = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
    SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage, 0.f);

    // 优化公式 3：暴击抗性降低暴击率 (假设 1点暴击抗性 直接扣除 1%暴击率)
    float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance;
    EffectiveCriticalHitChance = FMath::Max<float>(EffectiveCriticalHitChance, 0.f);

    // 判定是否暴击
    const bool bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;

    // 预留：如果你想让UI弹出“暴击”字样，解除下面注释并确保库函数存在
    UTaFeiAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);
    
    // 流水线加工：暴击触发，基础伤害翻倍 + 额外爆伤加成
    Damage = bCriticalHit ? (2.f * Damage + SourceCriticalHitDamage) : Damage;

    // =========================================================================
    // 【第六步】 输出最终结果到 IncomingDamage
    // =========================================================================
    if (Damage > 0.f)
    {
       OutExecutionModifiers.AddOutputModifier(FGameplayModifierEvaluatedData(UTaFeiAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage));
    }
}