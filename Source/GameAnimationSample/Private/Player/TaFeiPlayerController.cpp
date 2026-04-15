// Copyright Daisy


#include "Player/TaFeiPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "TaFeiGameplayTags.h"
#include "Input/TaFeiInputComponent.h"
#include "AbilitySystem/TaFeiAbilitySystemComponent.h"
#include "UI/Widget/TaFeiDamageTextComponent.h" 
#include "Player/TaFeiPlayerState.h"

ATaFeiPlayerController::ATaFeiPlayerController()
{
	bReplicates = true;

	
}

void ATaFeiPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	
	TryInitGAS();
}

void ATaFeiPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	
	TryInitGAS();
}

void ATaFeiPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	TryInitGAS();
}

void ATaFeiPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{
	
	
	if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		
		UTaFeiDamageTextComponent* DamageText = NewObject<UTaFeiDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent();
		
		
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		
		
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		
		
		DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit); // 这一步之后，在蓝图中完成最后一步（伤害不同颜色显示）
	}
}

void ATaFeiPlayerController::TryInitGAS()
{
	// 尝试获取灵魂和肉体
	ATaFeiPlayerState* TaFeiPS = GetPlayerState<ATaFeiPlayerState>();
	APawn* CurrentPawn = GetPawn();


	if (TaFeiPS && CurrentPawn)
	{
		TaFeiPS->InitializeGASForPawn(CurrentPawn);
	}
}
void ATaFeiPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 获取自定义的输入组件
	UTaFeiInputComponent* TaFeiInputComponent = CastChecked<UTaFeiInputComponent>(InputComponent);

	// 绑定技能输入！把按下的事件路由给我们的 AbilityInputTag 对应函数
	if (InputConfig)
	{
		TaFeiInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
	}
}


UTaFeiAbilitySystemComponent* ATaFeiPlayerController::GetASC()
{
	if (TaFeiASC == nullptr)
	{
		TaFeiASC = Cast<UTaFeiAbilitySystemComponent>(GetPlayerState<ATaFeiPlayerState>()->GetAbilitySystemComponent());
	}
	return TaFeiASC;
}

void ATaFeiPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (GetASC())
	{
		
		GetASC()->AbilityInputTagPressed(InputTag);
	}
}

void ATaFeiPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	
	if (UTaFeiAbilitySystemComponent* CurrentASC = GetASC())
	{
		
		CurrentASC->AbilityInputTagReleased(InputTag);
        
		
		if (InputTag.MatchesTagExact(FTaFeiGameplayTags::Get().InputTag_Ultimate))
		{
			
			FGameplayEventData Payload;
			
			CurrentASC->HandleGameplayEvent(FTaFeiGameplayTags::Get().Event_Ultimate_Release, &Payload);
		}
		else
		{
			
			//UE_LOG(LogTemp, Error, TEXT("松开的标签 [%s] 不是大招标签 [%s]！"), *InputTag.ToString(), *FTaFeiGameplayTags::Get().InputTag_Ultimate.ToString());
		}
	}
}

void ATaFeiPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetASC())
	{
		
		GetASC()->AbilityInputTagHeld(InputTag);
	}
}