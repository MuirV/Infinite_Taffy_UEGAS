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


	/* Event Tags ▼ */
	GameplayTags.Event_Melee_Hit = UGameplayTagsManager::Get().AddNativeGameplayTag(
	   FName("Event.Melee.Hit"), FString("Event triggered when melee attack hits"));

	GameplayTags.Event_Combo_WindowOpen = UGameplayTagsManager::Get().AddNativeGameplayTag(
	   FName("Event.Combo.WindowOpen"), FString("Window for next combo input opens"));

	GameplayTags.Event_Combo_WindowClosed = UGameplayTagsManager::Get().AddNativeGameplayTag(
	   FName("Event.Combo.WindowClosed"), FString("Window for next combo input closes"));

	GameplayTags.Event_Ultimate_Release = UGameplayTagsManager::Get().AddNativeGameplayTag(
	   FName("Event.Ultimate.Release"), FString("Event triggered when the ultimate key is released"));
}