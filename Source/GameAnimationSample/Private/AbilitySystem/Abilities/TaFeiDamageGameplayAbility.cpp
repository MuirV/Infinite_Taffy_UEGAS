// Copyright Daisy


#include "AbilitySystem/Abilities/TaFeiDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Interaction/TaFeiCombatInterface.h" // 引入你的接口
#include "AbilitySystemComponent.h"

void UTaFeiDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	// 安全检查
	if (!DamageEffectClass || !TargetActor) return;

	// 检查 TargetActor
	if (!TargetActor)
	{
		return;
	}

	//  检查 GE 类是否配置
	if (!DamageEffectClass)
	{
		return;
	}
	
	
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
	
	for (const TTuple<FGameplayTag, FScalableFloat>& Pair : DamageTypes)
	{
		
		const float ScaledDamage = Pair.Value.GetValueAtLevel(CurrentLevel);
		
		
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, ScaledDamage);

		FString Msg = FString::Printf(TEXT("计算前技能基础伤害 Tag: %s, 数值: %f"), *Pair.Key.ToString(), ScaledDamage);
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, Msg);
	}

	if (GetAvatarActorFromActorInfo()->HasAuthority()) 
	{
		
		GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), TargetASC);
	}
}

UAnimMontage* UTaFeiDamageGameplayAbility::RetrieveMontageFromAvatar()
{
	
	AActor* AvatarActor = GetAvatarActorFromActorInfo();

	// 日志 检查 GA 里的 CombatMontageTag 是不是空的
	UE_LOG(LogTemp, Warning, TEXT("GAS Log: GA trying to retrieve montage with Tag = %s"), *CombatMontageTag.ToString());

	if (AvatarActor && AvatarActor->Implements<UTaFeiCombatInterface>())
	{
		UAnimMontage* FoundMontage = ITaFeiCombatInterface::Execute_GetCombatMontageByTag(AvatarActor, CombatMontageTag);
       
		if(!FoundMontage)
		{
			UE_LOG(LogTemp, Error, TEXT("GAS Log: RetrieveMontage 失败 - 蓝图接口没返回对应 Tag 的蒙太奇！"));
		}
		return FoundMontage;
	}
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
