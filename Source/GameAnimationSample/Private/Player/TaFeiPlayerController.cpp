// Copyright Daisy


#include "Player/TaFeiPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Input/TaFeiInputComponent.h"
#include "AbilitySystem/TaFeiAbilitySystemComponent.h"
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
	if (GetASC())
	{
		
		GetASC()->AbilityInputTagReleased(InputTag);
	}
}

void ATaFeiPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetASC())
	{
		
		GetASC()->AbilityInputTagHeld(InputTag);
	}
}