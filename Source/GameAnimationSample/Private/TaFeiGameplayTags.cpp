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
	
	GameplayTags.Abilities_Attack_ComboF = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Attack.ComboF"), FString("F Key Combo"));
	
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


	/* Montage Tags */

	// LMB Combo
	GameplayTags.Montage_Attack_LMB_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.LMB.1"), FString("LMB Combo Step 1"));

	GameplayTags.Montage_Attack_LMB_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.LMB.2"), FString("LMB Combo Step 2"));

	GameplayTags.Montage_Attack_LMB_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.LMB.3"), FString("LMB Combo Step 3"));

	// F Combo
	GameplayTags.Montage_Attack_F_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.F.1"), FString("F Combo Step 1"));

	GameplayTags.Montage_Attack_F_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.F.2"), FString("F Combo Step 2"));

	// Mixed Combo
	GameplayTags.Montage_Attack_Mixed_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.Mixed.1"), FString("Mixed Combo Step 1"));

	GameplayTags.Montage_Attack_Mixed_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.Mixed.2"), FString("Mixed Combo Step 2"));

	GameplayTags.Montage_Attack_Mixed_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.Mixed.3"), FString("Mixed Combo Step 3"));

	// Ultimate
	GameplayTags.Montage_Ultimate = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Ultimate"), FString("Ultimate Montage"));
}