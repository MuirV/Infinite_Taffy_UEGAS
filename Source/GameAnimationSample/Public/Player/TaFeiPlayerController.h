// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TaFeiPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API ATaFeiPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATaFeiPlayerController();

protected:
	// 服务器端：附身完成时调用
	virtual void OnPossess(APawn* InPawn) override;

	// 客户端兜底 1：当肉体(Pawn)同步到客户端时触发
	virtual void OnRep_Pawn() override;

	// 客户端兜底 2：当灵魂(PlayerState)同步到客户端时触发
	virtual void OnRep_PlayerState() override;

private:
	// 核心逻辑：尝试初始化 GAS（防异步空指针）
	void TryInitGAS();	
};
