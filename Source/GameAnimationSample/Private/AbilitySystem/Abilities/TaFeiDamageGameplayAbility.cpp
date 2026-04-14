// Copyright Daisy


#include "AbilitySystem/Abilities/TaFeiDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Interaction/TaFeiCombatInterface.h" // 引入你的接口
#include "AbilitySystemComponent.h"
#include "TaFeiGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UTaFeiDamageGameplayAbility::UTaFeiDamageGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UTaFeiDamageGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!bAutoPlayMontage) 
	{
		return; 
	}
	
	ComboIndex = 1;
	bInputBuffered = false;
	bComboWindowOpen = false;
	bIsTransitioning = false;
	InputSequence.Empty();

	//  记录起手的输入标签，为后续派生连招做准备 （拓展）
	if (TriggerEventData)
	{
		InputSequence.Add(TriggerEventData->EventTag);
	}

	
	PlayComboMontage();
}

void UTaFeiDamageGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 清理状态
	ComboIndex = 1;
	bInputBuffered = false;
	bComboWindowOpen = false;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UTaFeiDamageGameplayAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	if (MontageTask) { MontageTask->EndTask(); }
    
	
	bIsTransitioning = false;
	
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	
}

void UTaFeiDamageGameplayAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	if (bComboWindowOpen)
	{
		bInputBuffered = true;
	}
	
}

void UTaFeiDamageGameplayAbility::PlayComboMontage()
{
	if (MontageTask)
	{
		MontageTask->OnCompleted.RemoveAll(this);
		MontageTask->OnInterrupted.RemoveAll(this);
		MontageTask->OnCancelled.RemoveAll(this);
		MontageTask->EndTask();
	}
	if (WaitOpenTask) { WaitOpenTask->EndTask(); }
	if (WaitCloseTask) { WaitCloseTask->EndTask(); }
	
	bIsTransitioning = false;
	
	UAnimMontage* MontageToPlay = RetrieveMontageFromAvatar();
	
	if (!MontageToPlay)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, MontageToPlay, 1.f);
	MontageTask->OnCompleted.AddDynamic(this, &UTaFeiDamageGameplayAbility::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UTaFeiDamageGameplayAbility::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &UTaFeiDamageGameplayAbility::OnMontageInterrupted);
	
	MontageTask->ReadyForActivation();

	WaitOpenTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FTaFeiGameplayTags::Get().Event_Combo_WindowOpen);
	WaitOpenTask->EventReceived.AddDynamic(this, &UTaFeiDamageGameplayAbility::OnComboWindowOpened);
	WaitOpenTask->ReadyForActivation();

	WaitCloseTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FTaFeiGameplayTags::Get().Event_Combo_WindowClosed);
	WaitCloseTask->EventReceived.AddDynamic(this, &UTaFeiDamageGameplayAbility::OnComboWindowClosed);
	WaitCloseTask->ReadyForActivation();
}

void UTaFeiDamageGameplayAbility::OnComboWindowOpened(FGameplayEventData Payload)
{
	bComboWindowOpen = true;
	bInputBuffered = false; 
}

void UTaFeiDamageGameplayAbility::OnComboWindowClosed(FGameplayEventData Payload)
{
	bComboWindowOpen = false;

	// 核心判定：在窗口期是否缓存了玩家输入，并且连段没有超过最大配置数（后续可拓展预输入）
	if (bInputBuffered && ComboIndex < MaxComboCount)
	{
		bIsTransitioning = true;
		
		ComboIndex++;            
		bInputBuffered = false;  
		
		
		PlayComboMontage();      
	}
	else
	{
		
	}
}

void UTaFeiDamageGameplayAbility::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UTaFeiDamageGameplayAbility::OnMontageInterrupted()
{
	// 修复 BUG：只有在被外力打断（比如受击、闪避）时，才结束技能。
	
	if (!bIsTransitioning)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

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

	
	if (AvatarActor && AvatarActor->Implements<UTaFeiCombatInterface>())
	{
		
		TArray<UAnimMontage*> MontageArray = ITaFeiCombatInterface::Execute_GetCombatMontagesByTag(AvatarActor, CombatMontageTag);
        
		
		if (MontageArray.Num() == 0)
		{
			return nullptr;
		}
		
		int32 ExpectedIndex = ComboIndex - 1;

		if (MontageArray.IsValidIndex(ExpectedIndex))
		{
			UAnimMontage* SelectedMontage = MontageArray[ExpectedIndex];
            
			if (SelectedMontage)
			{
				return SelectedMontage;
			}
		}
		else
		{
			
			return MontageArray[0]; 
		}
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
