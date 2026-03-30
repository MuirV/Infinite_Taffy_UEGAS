// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "TaFeiPlayerController.generated.h"

class UTaFeiInputConfig;
class UTaFeiAbilitySystemComponent;
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

	virtual void SetupInputComponent() override;
	
private:
	// 核心逻辑：尝试初始化 GAS（防异步空指针）
	void TryInitGAS();

	// 配置给蓝图填写的输入资产
	UPROPERTY(EditDefaultsOnly, Category="TaFei|Input")
	TObjectPtr<UTaFeiInputConfig> InputConfig;

	// 战斗逻辑的 IMC（可以在蓝图里配，优先级设为 1，盖过 GASP 的 0）
	UPROPERTY(EditDefaultsOnly, Category="TaFei|Input")
	TObjectPtr<class UInputMappingContext> CombatContext;

	
	// 回调函数：按键按下、松开、按住
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	// 缓存一个 ASC 指针，省得每次都去 Cast
	UPROPERTY()
	TObjectPtr<UTaFeiAbilitySystemComponent> TaFeiASC;

	UTaFeiAbilitySystemComponent* GetASC();
};
