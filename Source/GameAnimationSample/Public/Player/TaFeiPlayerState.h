// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "TaFeiPlayerState.generated.h"

/**
 * 
 */

class UTaFeiAbilitySystemComponent;
class UTaFeiAttributeSet;
UCLASS()
class GAMEANIMATIONSAMPLE_API ATaFeiPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ATaFeiPlayerState();

	// 必须实现 GAS 的核心接口
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
	UTaFeiAttributeSet* GetAttributeSet() const { return AttributeSet; }

	// Key : 供纯蓝图角色调用，完成 Owner 和 Avatar 的绑定 且暴露给蓝图，让 PC_Sandbox 在 Possess 之后调用
	UFUNCTION(BlueprintCallable, Category = "GAS|Initialization")
	void InitializeGASForPawn(APawn* AvatarPawn);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UTaFeiAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UTaFeiAttributeSet> AttributeSet;

	//防止重复初始化的安全锁
	bool bGASInitialized = false;
	
};
