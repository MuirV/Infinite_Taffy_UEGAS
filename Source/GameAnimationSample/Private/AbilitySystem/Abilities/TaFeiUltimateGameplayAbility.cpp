

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

	// 获取 CurveTable 中的基础伤害 ( DamageTypes 的 Damage.True 键下)
	float BaseDamage = 0.f;
	if (DamageTypes.Contains(FTaFeiGameplayTags::Get().Damage_True))
	{
		BaseDamage = DamageTypes[FTaFeiGameplayTags::Get().Damage_True].GetValueAtLevel(CurrentLevel);
	}

	//基础伤害的 2.5 次方
	float FinalDamage = FMath::Pow(BaseDamage, 2.5f);

	// 计算蓄力增伤系数 (Clamp 限制在 1.0 到 MaxChargeMultiplier 之间)
	float ChargeRatio = FMath::Clamp(ChargeTime / MaxChargeTime, 0.f, 1.f);
	// 线性插值：0秒蓄力=1倍，满蓄力=MaxChargeMultiplier倍
	float Multiplier = FMath::Lerp(1.0f, MaxChargeMultiplier, ChargeRatio); 

	FinalDamage *= Multiplier;

	// 打包进 GE，贴上 Damage.True 的标签
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, FTaFeiGameplayTags::Get().Damage_True, FinalDamage);

	// 应用给敌人
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), TargetASC);
}