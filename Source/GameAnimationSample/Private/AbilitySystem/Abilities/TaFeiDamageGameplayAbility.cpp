// Copyright Daisy


#include "AbilitySystem/Abilities/TaFeiDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UTaFeiDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	// 安全检查
	if (!DamageEffectClass || !TargetActor) return;

	// 获取受击者的 ASC (肉体如果是 BP_SandboxCharacter，这里依然能取到它挂在 PlayerState 上的 ASC)
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC) return;

	// 生成 GE 的实例 (Spec)
	// GetAbilityLevel() 会返回我们在 PlayerState 中赋予技能时传入的等级 (就是人物当前等级)
	const float CurrentLevel = GetAbilityLevel();
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, CurrentLevel);

	if (!DamageSpecHandle.IsValid()) return;

	//遍历我们配置的所有伤害类型 (比如这个技能既有物理伤害又有魔法伤害)
	for (const TTuple<FGameplayTag, FScalableFloat>& Pair : DamageTypes)
	{
		// 核心一步：拿着当前的技能等级，去 CurveTable 里查对应的数值！
		const float ScaledDamage = Pair.Value.GetValueAtLevel(CurrentLevel);
		
		// 通过 SetByCaller 把查出来的伤害数值“打包”进 GE 里，贴上对应的 Tag 标签
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, ScaledDamage);
	}

	//把打包好的伤害 GE 应用给目标
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), TargetASC);
}
