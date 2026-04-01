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

	// Event Tags  考虑到用Section做动画
	FGameplayTag Event_Melee_Hit;
	FGameplayTag Event_Combo_WindowOpen;
	FGameplayTag Event_Combo_WindowClosed;
	FGameplayTag Event_Ultimate_Release;    // 松开按键，释放大招事件 后续拓展能根据蓄力时间增加伤害(有最大上限2s达到最大)，且大招本身能够破防敌人

	// UI层面 角色属性反映
	// Primary Attributes
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;

	// Secondary Attributes
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;
	
protected:
	static FTaFeiGameplayTags GameplayTags;
};