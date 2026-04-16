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
	UTaFeiDamageGameplayAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	UFUNCTION(BlueprintCallable, Category = "TaFei|Combat")
	void CauseDamage(AActor* TargetActor);
	
	UFUNCTION(BlueprintCallable, Category = "TaFei|Combat")
	UAnimMontage* RetrieveMontageFromAvatar();

	
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TaFei|Combat")
	bool bAutoPlayMontage = true;  //是否交由cpp层面播动画，大招取消勾选
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TaFei|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TaFei|Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TaFei|Combat")
	FGameplayTag CombatMontageTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TaFei|Combo")
	int32 MaxComboCount = 3; 
	int32 ComboIndex = 1;
	
	bool bInputBuffered = false; 
	bool bComboWindowOpen = false;

	//bool bIsTransitioning = false;

	
	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* MontageTask;

	UPROPERTY()
	class UAbilityTask_WaitGameplayEvent* WaitOpenTask;

	UPROPERTY()
	class UAbilityTask_WaitGameplayEvent* WaitCloseTask;
	
	TArray<FGameplayTag> InputSequence;
	
	void PlayComboMontage();

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnComboWindowOpened(FGameplayEventData Payload);

	UFUNCTION()
	void OnComboWindowClosed(FGameplayEventData Payload);
	
	UFUNCTION(BlueprintPure)
	FTaFeiTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaFeiTaggedMontage>& TaggedMontages) const;

	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitInputTask;

	// 新增：当听到按键事件时触发的回调
	UFUNCTION()
	void OnComboInputReceived(FGameplayEventData Payload);

	// 新增：核心派生连招判定中心
	void EvaluateBranchingCombo();
};
