// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Interaction/TaFeiPlayerInterface.h"
#include "TaFeiPlayerState.generated.h"

/**
 * 
 */

// 定义 UI 需要监听的委托
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32 /*StatValue*/);

class UCharacterClassInfo;
class UTaFeiAbilitySystemComponent;
class UTaFeiAttributeSet;
UCLASS()
class GAMEANIMATIONSAMPLE_API ATaFeiPlayerState : public APlayerState, public ITaFeiPlayerInterface
{
	GENERATED_BODY()

public:
	ATaFeiPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// 必须实现 GAS 的核心接口
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
	UTaFeiAttributeSet* GetAttributeSet() const { return AttributeSet; }

	// Key : 供纯蓝图角色调用，完成 Owner 和 Avatar 的绑定 且暴露给蓝图，让 PC_Sandbox 在 Possess 之后调用
	UFUNCTION(BlueprintCallable, Category = "GAS|Initialization")
	void InitializeGASForPawn(APawn* AvatarPawn);

	// 简单实现获取等级的 Getter
	UFUNCTION(BlueprintCallable, Category = "TaFei|PlayerState")
	int32 GetPlayerLevel() const { return Level; }

	//  重构，这个要写在Gamemode里面
	// // 暴露给蓝图，用于在编辑器里把填好的 DataAsset 塞进来
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TaFei|GAS|Data")
	// TObjectPtr<UCharacterClassInfo> CharacterData;
	
	// 给蓝图配置的升级数据
	UPROPERTY(EditDefaultsOnly, Category = "TaFei|LevelUp")
	TObjectPtr<ULevelUpInfo> LevelUpInfo;

	FOnPlayerStatChanged OnXPChangedDelegate;
	FOnPlayerStatChanged OnLevelChangedDelegate;
	FOnPlayerStatChanged OnAttributePointsChangedDelegate;
	FOnPlayerStatChanged OnSpellPointsChangedDelegate;
	
	// ... 实现 ITaFeiPlayerInterface 的接口 ...
	virtual int32 GetXP_Implementation() const override;
	virtual int32 FindLevelForXP_Implementation(int32 InXP) const override;
	virtual int32 GetAttributePointsReward_Implementation(int32 InLevel) const override;
	virtual int32 GetSpellPointsReward_Implementation(int32 InLevel) const override;
	virtual void AddToPlayerLevel_Implementation(int32 InPlayerLevel) override;
	virtual void AddToAttributePoints_Implementation(int32 InAttributePoints) override;
	virtual void AddToSpellPoints_Implementation(int32 InSpellPoints) override;
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual int32 GetSpellPoints_Implementation() const override;
	
	void SetXP(int32 InXP);
	void SetToLevel(int32 InLevel);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UTaFeiAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UTaFeiAttributeSet> AttributeSet;

	//防止重复初始化的安全锁
	bool bGASInitialized = false;
	
	// 标记这个 PlayerState 属于什么职业（玩家自然是 Player）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TaFei|GAS|Data")
	ETaFeiCharacterClass CharacterClass = ETaFeiCharacterClass::Player;

	// 网络同步变量
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = "OnRep_Level", Category = "TaFei|PlayerStats")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = "OnRep_XP", Category = "TaFei|PlayerStats")
	int32 XP = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = "OnRep_AttributePoints", Category = "TaFei|PlayerStats")
	int32 AttributePoints = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = "OnRep_SpellPoints", Category = "TaFei|PlayerStats")
	int32 SpellPoints = 1;
	
	// 辅助函数：初始化属性 (应用 GE)
	void InitializeAttributes();
    
	// 辅助函数：赋予初始技能 (授予 GA)
	void AddStartupAbilities();

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);
	
	UFUNCTION()
	void OnRep_AttributePoints(int32 OldAttributePoints);

	UFUNCTION()
	void OnRep_SpellPoints(int32 OldLevel);
	
	UFUNCTION()
	void OnRep_XP(int32 OldXP);
};
