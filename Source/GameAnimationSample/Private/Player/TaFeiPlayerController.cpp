// Copyright Daisy


#include "Player/TaFeiPlayerController.h"

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
