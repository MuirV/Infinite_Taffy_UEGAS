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
	// ---------- 获取基础伤害 ----------
    float Damage = Spec.GetSetByCallerMagnitude(FTaFeiGameplayTags::Get().Damage_Physical, false, 0.f);

	
    // =========================================================================
    // Buff应用增伤与减伤 （后续可拓展使用道具后增伤减伤等等）
    // =========================================================================
	
    float SourceDamageMultiplier = 0.f; // 读出来的基础是加成比例 (如 0.2 代表 +20%伤害)
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageMultiplierDef, EvaluationParameters, SourceDamageMultiplier);
    SourceDamageMultiplier = FMath::Max<float>(SourceDamageMultiplier, 0.f); // 确保不会变成负增伤

    float TargetDamageReduction = 0.f;  
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageReductionDef, EvaluationParameters, TargetDamageReduction);
    TargetDamageReduction = FMath::Clamp<float>(TargetDamageReduction, 0.f, 1.f); // 0~1 之间的减伤

	// =========================================================================
	// 咒术状态开启时增伤或者减伤
	// =========================================================================
    Damage = Damage * (1.f + SourceDamageMultiplier) * (1.f - TargetDamageReduction);
	
	// 判断是否拥有State_CursedMode 这个tag（咒术形态）
	const FGameplayTag CursedModeTag = FTaFeiGameplayTags::Get().State_CursedMode;

	bool bSourceHasCursedMode = EvaluationParameters.SourceTags && EvaluationParameters.SourceTags->HasTag(CursedModeTag);
	bool bTargetHasCursedMode = EvaluationParameters.TargetTags && EvaluationParameters.TargetTags->HasTag(CursedModeTag);
	// 如果攻击者处于咒术形态，最终伤害增加 50%
	if (bSourceHasCursedMode)
	{
		Damage *= 1.5f; 
	}

	// 如果受击者处于咒术形态，受到的最终伤害减少 90%
	if (bTargetHasCursedMode)
	{
		Damage *= 0.1f; 
	}
    // =========================================================================
    // 结算 格挡 (Block)
    // =========================================================================
    float TargetBlockChance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
    TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);

    const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
    
    FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
    UTaFeiAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
    
    Damage = bBlocked ? Damage / 2.f : Damage;

    // =========================================================================
    // 结算 护甲与破甲 
    // =========================================================================
    float TargetArmor = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
    TargetArmor = FMath::Max<float>(TargetArmor, 0.f);

    float SourceArmorPenetration = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
    SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.f);

    float ArmorPenetrationPct = FMath::Clamp(SourceArmorPenetration / 100.f, 0.f, 1.f);
    float EffectiveArmor = TargetArmor * (1.f - ArmorPenetrationPct);
    EffectiveArmor = FMath::Max<float>(EffectiveArmor, 0.f);
    
    float ArmorMitigation = FMath::Clamp(EffectiveArmor, 0.f, 85.f);
    Damage *= (100.f - ArmorMitigation) / 100.f;

    // =========================================================================
    //  结算 暴击 (Critical Hit) 
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

    float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance;
    EffectiveCriticalHitChance = FMath::Max<float>(EffectiveCriticalHitChance, 0.f);

    const bool bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;
    
    UTaFeiAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);
    
    Damage = bCriticalHit ? (2.f * Damage + SourceCriticalHitDamage) : Damage;

	// =========================================================================
	// 结算 无敌帧 与 完美闪避判定
	// =========================================================================
	bool bTargetIsInvincible = EvaluationParameters.TargetTags && EvaluationParameters.TargetTags->HasTagExact(FTaFeiGameplayTags::Get().State_Invincible);
	bool bTargetIsDodging = EvaluationParameters.TargetTags && EvaluationParameters.TargetTags->HasTagExact(FTaFeiGameplayTags::Get().State_Movement_Dodge);

	if (bTargetIsInvincible)
	{
		Damage = 0.f; // 无敌状态，伤害归零
        
		// 如果此刻正在闪避，那就是完美闪避
		if (bTargetIsDodging)
		{
			UTaFeiAbilitySystemLibrary::SetIsPerfectDodge(EffectContextHandle, true);
		}
	}

	// =========================================================================
	// 输出最终结果
	// =========================================================================
	// 即使 Damage 是 0，如果是完美闪避，我们也必须输出这个 0 伤害修改器！
	// 否则 PostGameplayEffectExecute 不会触发，我们就截获不到这个事件了。
	if (Damage > 0.f || UTaFeiAbilitySystemLibrary::IsPerfectDodge(EffectContextHandle))
	{
		OutExecutionModifiers.AddOutputModifier(FGameplayModifierEvaluatedData(UTaFeiAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage));
	}
}