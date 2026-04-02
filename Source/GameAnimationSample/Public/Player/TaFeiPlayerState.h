// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "TaFeiPlayerState.generated.h"

/**
 * 
 */

class UCharacterClassInfo;
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

	// 简单实现获取等级的 Getter
	UFUNCTION(BlueprintCallable, Category = "TaFei|PlayerState")
	int32 GetPlayerLevel() const { return Level; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UTaFeiAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UTaFeiAttributeSet> AttributeSet;

	//防止重复初始化的安全锁
	bool bGASInitialized = false;

	// 暴露给蓝图，用于在编辑器里把填好的 DataAsset 塞进来
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TaFei|GAS|Data")
	TObjectPtr<UCharacterClassInfo> CharacterData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TaFei|PlayerState")
	int32 Level = 1; // 默认 1 级
	
	// 辅助函数：初始化属性 (应用 GE)
	void InitializeAttributes();
    
	// 辅助函数：赋予初始技能 (授予 GA)
	void AddStartupAbilities();
};
