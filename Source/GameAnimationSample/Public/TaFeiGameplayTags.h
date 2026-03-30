// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * 原生 GameplayTags 单例
 */
struct FTaFeiGameplayTags
{
public:
	static const FTaFeiGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();

	// ---------------- 输入标签 (Input) ----------------
	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_F;
	FGameplayTag InputTag_Ultimate;

	// ---------------- 伤害标签 (Damage) ----------------
	FGameplayTag Damage_Physical; // 物理伤害

	// ---------------- 技能标签 (Abilities) ----------------
	// 连招系统
	FGameplayTag Abilities_Attack_ComboLMB;   // 连续左键
	FGameplayTag Abilities_Attack_ComboF;   // 连续 F 键
	FGameplayTag Abilities_Attack_ComboMixed; // 派生连招：左键1下 -> F键2下
	// 大招
	FGameplayTag Abilities_Ultimate;          // 大招释放
	
	// ---------------- 状态标签 (Status/Buffs) ----------------
	FGameplayTag Buff_DamageAmplification;    // 伤害增幅
	FGameplayTag Buff_DamageReduction;        // 90%减伤

	// ---------------- 属性标签 (Attributes) ----------------
	FGameplayTag Attributes_Vital_Health;
	FGameplayTag Attributes_Vital_UltimateEnergy; // 大招能量

protected:
	static FTaFeiGameplayTags GameplayTags;
};