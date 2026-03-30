// Copyright Daisy

#include "TaFeiGameplayTags.h"
#include "GameplayTagsManager.h"

FTaFeiGameplayTags FTaFeiGameplayTags::GameplayTags;

void FTaFeiGameplayTags::InitializeNativeGameplayTags()
{
	/* 输入 Tags */
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.LMB"), FString("Input Tag for Left Mouse Button"));
		
	GameplayTags.InputTag_F = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.F"), FString("Input Tag for F Key"));

	GameplayTags.InputTag_Ultimate = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Ultimate"), FString("Input Tag for Ultimate Ability"));

	/* 伤害 Tags */
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Physical"), FString("Physical Damage Type"));
	
	/* 技能 Tags */
	GameplayTags.Abilities_Attack_ComboLMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Attack.ComboLMB"), FString("Left Mouse Button Combo"));

	// 注册 F 键连招
	GameplayTags.Abilities_Attack_ComboF = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Attack.ComboF"), FString("F Key Combo"));

	// 注册混合派生连招 (LMB -> F -> F)
	GameplayTags.Abilities_Attack_ComboMixed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Attack.ComboMixed"), FString("Mixed Combo: LMB once then F twice"));

	GameplayTags.Abilities_Ultimate = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Ultimate"), FString("Ultimate Skill"));

	/* Buff Tags */
	GameplayTags.Buff_DamageAmplification = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Buff.DamageAmplification"), FString("Damage Amp Buff after specific skill"));

	GameplayTags.Buff_DamageReduction = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Buff.DamageReduction"), FString("90% Damage Reduction Buff"));
        
	/* Attribute Tags */
	GameplayTags.Attributes_Vital_UltimateEnergy = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Vital.UltimateEnergy"), FString("Amount of Ultimate Energy"));
}