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

	// 服务器：第一时间尝试初始化
	TryInitGAS();
}

void ATaFeiPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	// 客户端兜底：Pawn 先到的情况
	TryInitGAS();
}

void ATaFeiPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	// 客户端兜底：PlayerState 先到的情况
	TryInitGAS();
}

void ATaFeiPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{
	
	// 安全检查：确保在本地 Controller，并且配置了类、目标没消失
	if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		// 动态创建一个 DamageTextComponent
		UTaFeiDamageTextComponent* DamageText = NewObject<UTaFeiDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent();
		
		// 先贴在受击者的 Root 组件上，获取正确的位置
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		
		// 紧接着分离它，保持它在世界空间的位置（这样角色跑开了，字依然会在受击瞬间的位置向上飘）
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		
		// 调用你组件里（蓝图中实现）的设置文字和播放动画的函数
		DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit); // 这一步之后，在蓝图中完成最后一步（伤害不同颜色显示）
	}
}

void ATaFeiPlayerController::TryInitGAS()
{
	// 尝试获取灵魂和肉体
	ATaFeiPlayerState* TaFeiPS = GetPlayerState<ATaFeiPlayerState>();
	APawn* CurrentPawn = GetPawn();

	// 只有当两者在当前端都已经准备好时，才调用初始化
	// 如果某个还没同步过来（nullptr），就忽略，等待另一个的 OnRep 触发
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

// 辅助函数：安全获取 ASC
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
		// 调用我们自定义的 Tag 版函数
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
			
			UE_LOG(LogTemp, Error, TEXT("松开的标签 [%s] 不是大招标签 [%s]！"), *InputTag.ToString(), *FTaFeiGameplayTags::Get().InputTag_Ultimate.ToString());
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