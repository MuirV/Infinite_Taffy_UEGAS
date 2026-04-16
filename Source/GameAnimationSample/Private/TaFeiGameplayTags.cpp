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

	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.1"), FString("Input Tag for CursedMode"));

	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.2"), FString("Input Tag for ReverseMode"));

	GameplayTags.InputTag_Shift = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Shift"), FString("Input Tag for Dodge?"));

	GameplayTags.InputTag_Q = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Q"), FString("Input Tag for Dodge"));
	
	/* 伤害 Tags */
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Physical"), FString("Physical Damage Type"));

	GameplayTags.Damage_True = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage.True"), FString("True Damage Type, ignores armor"));
	
	/* 技能 Tags */
	GameplayTags.Abilities_Attack_ComboLMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Attack.ComboLMB"), FString("Left Mouse Button Combo"));
	
	GameplayTags.Abilities_Attack_ComboF = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Attack.ComboF"), FString("F Key Combo"));
	
	GameplayTags.Abilities_Attack_ComboMixed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Attack.ComboMixed"), FString("Mixed Combo: LMB once then F twice"));

	GameplayTags.Abilities_Attack_Monster = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Abilities.Attack.Monster"), FString("Monster Attack"));
	
	GameplayTags.Abilities_Ultimate = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Ultimate"), FString("Ultimate Skill"));

	GameplayTags.Abilities_Dodge = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Dodge"), FString("Dodge Skill"));

	
	/* Buff Tags */
	GameplayTags.Buff_DamageAmplification = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Buff.DamageAmplification"), FString("Damage Amp Buff after specific skill"));

	GameplayTags.Buff_DamageReduction = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Buff.DamageReduction"), FString("90% Damage Reduction Buff"));

	/*
	* Meta Attributes
	*/
	GameplayTags.Attributes_Meta_IncomingXP = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Meta.IncomingXP"),
		FString("Meta Incoming XP")
	);

	
	/* Attribute Tags */
	GameplayTags.Attributes_Vital_UltimateEnergy = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Vital.UltimateEnergy"), FString("Amount of Ultimate Energy"));

	/* State Tags */
	GameplayTags.State_Movement_Sprinting = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Movement.Sprinting"), FString("Character is currently sprinting"));

	GameplayTags.State_Movement_Airborne = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Movement.Airborne"), FString("Character is in the air"));

	GameplayTags.State_Movement_Dodge = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Movement.Dodge"), FString("Character is Dodging"));

	GameplayTags.State_CursedMode = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.CursedMode"), FString("Character is in CursedMode"));

	GameplayTags.State_ReverseMode = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.ReverseMode"), FString("Character is in ReverseMode"));

	GameplayTags.State_Invincible = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Invincible"), FString("Character is Invincible"));

	GameplayTags.State_Attack_Active = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Attack.Active"), FString("Character is Attacking"));

	/* Cooldown Tags */
	GameplayTags.Cooldown_Ultimate = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Cooldown.Ultimate"), FString("Ultimate Cooldown"));

	GameplayTags.Cooldown_Dodge = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Cooldown.Dodge"), FString("Dodge Cooldown"));

	GameplayTags.Cooldown_CursedMode = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Cooldown.CursedMode"), FString("Cursed Cooldown"));

	GameplayTags.Cooldown_ReverseMode = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Cooldown.ReverseMode"), FString("Reverse Cooldown"));
	
	/* Event Tags ▼ */
	GameplayTags.Event_Melee_Hit = UGameplayTagsManager::Get().AddNativeGameplayTag(
	   FName("Event.Melee.Hit"), FString("Event triggered when melee attack hits"));

	GameplayTags.Event_Combo_WindowOpen = UGameplayTagsManager::Get().AddNativeGameplayTag(
	   FName("Event.Combo.WindowOpen"), FString("Window for next combo input opens"));

	GameplayTags.Event_Combo_WindowClosed = UGameplayTagsManager::Get().AddNativeGameplayTag(
	   FName("Event.Combo.WindowClosed"), FString("Window for next combo input closes"));

	GameplayTags.Event_Ultimate_Release = UGameplayTagsManager::Get().AddNativeGameplayTag(
	   FName("Event.Ultimate.Release"), FString("Event triggered when the ultimate key is released"));

	GameplayTags.Event_CursedMode_AllowClose = UGameplayTagsManager::Get().AddNativeGameplayTag(
	   FName("Event.CursedMode.AllowClose"), FString("Event that allow us close CursedMode"));

	GameplayTags.Event_ReverseMode_AllowClose = UGameplayTagsManager::Get().AddNativeGameplayTag(
	   FName("Event.ReverseMode.AllowClose"), FString("Event that allow us close ReverseMode"));
	
	GameplayTags.Event_Combat_PerfectDodge = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Combat.PerfectDodge"), FString("Event triggered upon a successful perfect dodge"));

	GameplayTags.Event_Combo_Branch_Mixed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Combo.Branch.Mixed"), FString("Event ComboMixed Activate"));
	

	/* Gameplay Cue */ 
	GameplayTags.GameplayCue_Combat_PerfectDodge = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.Combat.PerfectDodge"), FString("Play perfect dodge sound and visual"));
        
	// UI 层面
	// ---------------- Primary Attributes ----------------

	GameplayTags.Attributes_Primary_Strength =
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Primary.Strength"),
			FString("Increase physical damage")
		);

	GameplayTags.Attributes_Primary_Intelligence =
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Primary.Intelligence"),
			FString("Increase magical power")
		);

	GameplayTags.Attributes_Primary_Resilience =
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Primary.Resilience"),
			FString("Increase armor and resistance")
		);

	GameplayTags.Attributes_Primary_Vigor =
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Primary.Vigor"),
			FString("Increase health")
		);

	// ---------------- Secondary Attributes ----------------

	GameplayTags.Attributes_Secondary_Armor =
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Secondary.Armor"),
			FString("Reduces incoming damage")
		);

	GameplayTags.Attributes_Secondary_ArmorPenetration =
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Secondary.ArmorPenetration"),
			FString("Ignores enemy armor")
		);

	GameplayTags.Attributes_Secondary_BlockChance =
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Secondary.BlockChance"),
			FString("Chance to block attacks")
		);

	GameplayTags.Attributes_Secondary_CriticalHitChance =
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Secondary.CriticalHitChance"),
			FString("Chance to deal critical damage")
		);

	GameplayTags.Attributes_Secondary_CriticalHitDamage =
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Secondary.CriticalHitDamage"),
			FString("Critical damage multiplier")
		);

	GameplayTags.Attributes_Secondary_CriticalHitResistance =
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Secondary.CriticalHitResistance"),
			FString("Reduces enemy critical chance")
		);

	GameplayTags.Attributes_Secondary_HealthRegeneration =
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Secondary.HealthRegeneration"),
			FString("Health regen per second")
		);

	GameplayTags.Attributes_Secondary_ManaRegeneration =
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Secondary.ManaRegeneration"),
			FString("Mana regen per second")
		);

	GameplayTags.Attributes_Secondary_MaxHealth =
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Secondary.MaxHealth"),
			FString("Maximum health")
		);

	GameplayTags.Attributes_Secondary_MaxMana =
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Secondary.MaxMana"),
			FString("Maximum mana")
		);

	/*
	 * Effects
	 */
    
	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
   FName("Effects.HitReact"),
   FString("Tag granted when Hit Reacting")
   );
	
}