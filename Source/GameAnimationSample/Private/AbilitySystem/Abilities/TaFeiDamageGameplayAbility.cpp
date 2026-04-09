// Copyright Daisy


#include "AbilitySystem/Abilities/TaFeiDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Interaction/TaFeiCombatInterface.h" // 引入你的接口
#include "AbilitySystemComponent.h"

void UTaFeiDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	// 安全检查
	if (!DamageEffectClass || !TargetActor) return;

	// 1. 检查 TargetActor
	if (!TargetActor)
	{
		return;
	}

	// 2. 检查 GE 类是否配置
	if (!DamageEffectClass)
	{
		return;
	}
	
	// 获取受击者的 ASC (肉体如果是 BP_SandboxCharacter，这里依然能取到它挂在 PlayerState 上的 ASC)
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC)
	{
		return;
	}

	// 生成 GE 的实例 (Spec)
	// GetAbilityLevel() 会返回我们在 PlayerState 中赋予技能时传入的等级 (就是人物当前等级)
	const float CurrentLevel = GetAbilityLevel();
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, CurrentLevel);

	if (!DamageSpecHandle.IsValid())
	{
		return;
	}

	//遍历我们配置的所有伤害类型 (比如这个技能既有物理伤害又有魔法伤害)
	for (const TTuple<FGameplayTag, FScalableFloat>& Pair : DamageTypes)
	{
		// 根据当前的技能等级，去 CurveTable 里查对应的数值
		const float ScaledDamage = Pair.Value.GetValueAtLevel(CurrentLevel);
		
		// 通过 SetByCaller 把查出来的伤害数值“打包”进 GE 里，贴上对应的 Tag 标签
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, ScaledDamage);

	
		// 打印具体打包了多少伤害
		FString Msg = FString::Printf(TEXT("计算前技能基础伤害 Tag: %s, 数值: %f"), *Pair.Key.ToString(), ScaledDamage);
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, Msg);
	}

	if (GetAvatarActorFromActorInfo()->HasAuthority()) 
	{
		// 现在 TargetASC 已经定义，可以正常解析了
		GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), TargetASC);
	}
}

UAnimMontage* UTaFeiDamageGameplayAbility::RetrieveMontageFromAvatar()
{
	// 获取肉体 (BP_SandboxCharacter)
	AActor* AvatarActor = GetAvatarActorFromActorInfo();

	// 检查肉体是否实现了我们的战斗接口
	if (AvatarActor && AvatarActor->Implements<UTaFeiCombatInterface>())
	{
		// 通过接口向角色索要蒙太奇
		return ITaFeiCombatInterface::Execute_GetCombatMontageByTag(AvatarActor, CombatMontageTag);
	}
	
	// 如果角色没实现接口，或者找不到，返回空
	return nullptr;
}


FTaFeiTaggedMontage UTaFeiDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaFeiTaggedMontage>& TaggedMontages) const
{
	if (TaggedMontages.Num() > 0)
	{
		const int32 Selection = FMath::RandRange(0, TaggedMontages.Num() - 1);
		return TaggedMontages[Selection];
	}
	
	
	return FTaFeiTaggedMontage();
}
