

#include "AbilitySystem/Abilities/TaFeiUltimateGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "TaFeiGameplayTags.h"

UTaFeiUltimateGameplayAbility::UTaFeiUltimateGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	bAutoPlayMontage = false;
}

void UTaFeiUltimateGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                    const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	
}

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

	
	float PowerDamage = FMath::Pow(BaseDamage, 2.5f);
	float ClampedChargeTime = FMath::Clamp(ChargeTime, 0.f, MaxChargeTime);
	
	float Multiplier = ChargeDamageMultiplierCurve.GetValueAtLevel(ClampedChargeTime);

	float FinalDamage = PowerDamage * Multiplier;

	
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, FTaFeiGameplayTags::Get().Damage_True, FinalDamage);

	
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), TargetASC);
}
