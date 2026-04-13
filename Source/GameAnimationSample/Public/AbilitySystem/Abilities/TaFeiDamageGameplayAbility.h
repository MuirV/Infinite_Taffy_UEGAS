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
	
	UFUNCTION(BlueprintCallable, Category = "TaFei|Combat")
	void CauseDamage(AActor* TargetActor);

	
	UFUNCTION(BlueprintCallable, Category = "TaFei|Combat")
	UAnimMontage* RetrieveMontageFromAvatar();

	
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TaFei|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TaFei|Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TaFei|Combat")
	FGameplayTag CombatMontageTag;

	UFUNCTION(BlueprintPure)
	FTaFeiTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaFeiTaggedMontage>& TaggedMontages) const;
};
