

#include "AbilitySystem/Abilities/TaFeiUltimateGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "TaFeiGameplayTags.h"

void UTaFeiUltimateGameplayAbility::CauseUltimateDamage(AActor* TargetActor, float ChargeTime)
{
	if (!DamageEffectClass || !TargetActor) return;

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC) return;

	const float CurrentLevel = GetAbilityLevel();
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, CurrentLevel);
	if (!DamageSpecHandle.IsValid()) return;

	
	float BaseDamage = 0.f;
	if (DamageTypes.Contains(FTaFeiGameplayTags::Get().Damage_True))
	{
		BaseDamage = DamageTypes[FTaFeiGameplayTags::Get().Damage_True].GetValueAtLevel(CurrentLevel);
	}

	
	float FinalDamage = FMath::Pow(BaseDamage, 2.5f);
	float ChargeRatio = FMath::Clamp(ChargeTime / MaxChargeTime, 0.f, 1.f);
	float Multiplier = FMath::Lerp(1.0f, MaxChargeMultiplier, ChargeRatio); 

	FinalDamage *= Multiplier;

	
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, FTaFeiGameplayTags::Get().Damage_True, FinalDamage);

	
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), TargetASC);
}