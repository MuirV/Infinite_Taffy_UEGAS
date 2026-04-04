// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "TaFeiPlayerController.generated.h"

class UTaFeiDamageTextComponent;
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

	// 客户端 RPC：只在发起攻击的玩家本地机器上显示伤害飘字
	UFUNCTION(Client, Reliable)
	void ClientShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);
	
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

	// 在 BP_TaFeiPlayerController 蓝图中配置你的 Widget Component 类
	UPROPERTY(EditDefaultsOnly, Category="TaFei|DamageText")
	TSubclassOf<UTaFeiDamageTextComponent> DamageTextComponentClass;

};
