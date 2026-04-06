// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ScalableFloat.h"
#include "Interaction/TaFeiCombatInterface.h"
#include "TaFeiDamageGameplayAbility.generated.h"

/**
 * 所有造成伤害的技能（普攻、连招、大招）的基类
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API UTaFeiDamageGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	// 暴露给蓝图调用的函数，传入受击的 Actor 即可造成伤害
	UFUNCTION(BlueprintCallable, Category = "TaFei|Combat")
	void CauseDamage(AActor* TargetActor);

	// ★ 新增：给蓝图 GA 提供一个快捷获取蒙太奇的节点
	UFUNCTION(BlueprintCallable, Category = "TaFei|Combat")
	UAnimMontage* RetrieveMontageFromAvatar();

protected:
	// 指定这个技能要应用哪个 GameplayEffect 来扣血（通常填一个只有 SetByCaller 属性的 GE）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TaFei|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// 核心：伤害类型 Tag 到 可缩放浮点数 的映射字典
	// 比如：Key = "Damage.Physical", Value = 查表的 ScalableFloat
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TaFei|Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;

	// ★ 新增：这个技能专属的连招蒙太奇标签
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TaFei|Combat")
	FGameplayTag CombatMontageTag;

	UFUNCTION(BlueprintPure)
	FTaFeiTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaFeiTaggedMontage>& TaggedMontages) const;
};
