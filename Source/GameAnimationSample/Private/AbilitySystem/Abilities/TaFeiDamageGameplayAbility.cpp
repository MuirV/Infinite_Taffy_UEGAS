// Copyright Daisy


#include "AbilitySystem/Abilities/TaFeiDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Interaction/TaFeiCombatInterface.h" // 引入你的接口
#include "AbilitySystemComponent.h"
#include "TaFeiGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameFramework/PlayerState.h"

UTaFeiDamageGameplayAbility::UTaFeiDamageGameplayAbility() : MontageTask(nullptr), WaitOpenTask(nullptr),
                                                             WaitCloseTask(nullptr),
                                                             WaitInputTask(nullptr)
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
	
	const UTaFeiDamageGameplayAbility* DefaultObj = GetClass()->GetDefaultObject<UTaFeiDamageGameplayAbility>();
	if (DefaultObj)
	{
		CombatMontageTag = DefaultObj->CombatMontageTag; 
		MaxComboCount = DefaultObj->MaxComboCount;      
	}
	
	ComboIndex = 1;
	bInputBuffered = false;
	bComboWindowOpen = false;
	InputSequence.Empty();

	
	if (TriggerEventData && TriggerEventData->EventTag.IsValid())
	{
		InputSequence.Add(TriggerEventData->EventTag);
	}
	else if (const FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec())
	{
		for (const FGameplayTag& Tag : Spec->GetDynamicSpecSourceTags())
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
			{
				InputSequence.Add(Tag);
				UE_LOG(LogTemp, Warning, TEXT("起手记录输入: %s"), *Tag.ToString());
				break;
			}
		}
	}
	
	PlayComboMontage();
}

void UTaFeiDamageGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
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
	if (WaitInputTask) { WaitInputTask->EndTask(); }

	if (bWasCancelled && ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		// 防止 Super::EndAbility 因为竞态没清干净标签
		ActorInfo->AbilitySystemComponent->RemoveLooseGameplayTag(FTaFeiGameplayTags::Get().State_Attack_Active);
	}
	
	ComboIndex = 1;
	bInputBuffered = false;
	bComboWindowOpen = false;
	
	InputSequence.Empty();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UTaFeiDamageGameplayAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	
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
	if (WaitInputTask) { WaitInputTask->EndTask(); }
	
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

	// 启动输入监听 Task！监听所有 "InputTag" 子标签（
	if (WaitInputTask) { WaitInputTask->EndTask(); } 
	WaitInputTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,
	FGameplayTag::RequestGameplayTag(FName("InputTag")), 
	nullptr, 
	false, 
	false
		);
	WaitInputTask->EventReceived.AddDynamic(this, &UTaFeiDamageGameplayAbility::OnComboInputReceived);
	WaitInputTask->ReadyForActivation();
}

void UTaFeiDamageGameplayAbility::OnComboWindowOpened(FGameplayEventData Payload)
{
	bComboWindowOpen = true;
	bInputBuffered = false; 
}

void UTaFeiDamageGameplayAbility::OnComboWindowClosed(FGameplayEventData Payload)
{
	bComboWindowOpen = false;
	
	if (bInputBuffered)
	{
		EvaluateBranchingCombo();

		
		if (!IsActive())
		{
			return; 
		}
		
		if (ComboIndex < MaxComboCount)
		{
			ComboIndex++;            
			bInputBuffered = false;  
			PlayComboMontage();      
		}
	}
}

void UTaFeiDamageGameplayAbility::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UTaFeiDamageGameplayAbility::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	
}

void UTaFeiDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	// 安全检查
	if (!DamageEffectClass || !TargetActor) return;

	
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
    
	
	if (!TargetASC)
	{
		if (APawn* TargetPawn = Cast<APawn>(TargetActor))
		{
			if (APlayerState* PS = TargetPawn->GetPlayerState())
			{
				TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PS);
			}
		}
	}


	if (!TargetASC)
	{
		return; //打中静态网格体了
	}

	// 生成 GE 的实例 (Spec)
	// GetAbilityLevel() 会返回我们在 PlayerState 中赋予技能时传入的等级 
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

void UTaFeiDamageGameplayAbility::OnComboInputReceived(FGameplayEventData Payload)
{
	
	if (bComboWindowOpen && !bInputBuffered)
	{
		bInputBuffered = true;
		
		InputSequence.Add(Payload.EventTag); 
		
		//EvaluateBranchingCombo();
	}
}

void UTaFeiDamageGameplayAbility::EvaluateBranchingCombo()
{
	const FTaFeiGameplayTags& Tags = FTaFeiGameplayTags::Get();


	FString SequenceStr;
	for (const FGameplayTag& Tag : InputSequence) { SequenceStr += Tag.ToString() + TEXT(", "); }
	UE_LOG(LogTemp, Warning, TEXT("当前按键记录 (Size:%d): [%s]"), InputSequence.Num(), *SequenceStr);
    
	// 派生路线 LMB + F
	if (InputSequence.Num() == 2)
	{
		if (InputSequence[0].MatchesTagExact(Tags.InputTag_LMB) && 
		   InputSequence[1].MatchesTagExact(Tags.InputTag_F))
		{
			
			UAbilitySystemComponent* CachedASC = GetAbilitySystemComponentFromActorInfo();
			
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			
			if (CachedASC)
			{
				FGameplayEventData Payload;
				
				Payload.EventTag = Tags.Event_Combo_Branch_Mixed; 

				//由于我们的PlayerState是Owner，而且我们用的纯蓝图角色，Avatar
				//UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActorFromActorInfo(), Tags.Event_Combo_Branch_Mixed, Payload);

				CachedASC->HandleGameplayEvent(Tags.Event_Combo_Branch_Mixed, &Payload);
			}
		}
	}
	if (CombatMontageTag.MatchesTagExact(Tags.Abilities_Attack_ComboMixed))
	{
		//  [LMB, F, F]
		if (InputSequence.Num() == 3)
		{
			UE_LOG(LogTemp, Warning, TEXT("检测到深层派生: LMB + F + F"));
			
		}
	}
}