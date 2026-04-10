// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h" // 必须包含这个才能用 FGameplayTag
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "TaFeiCombatInterface.generated.h"


USTRUCT(BlueprintType)
struct FTaFeiTaggedMontage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* Montage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FGameplayTag MontageTag; // 比如：Abilities.Attack.ComboLMB

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag SocketTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* ImpactSound = nullptr;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UTaFeiCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GAMEANIMATIONSAMPLE_API ITaFeiCombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	// 获取当前角色的等级 (纯虚函数，需要在 BP 或 C++ 中实现)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat Interface")
	int32 GetPlayerLevel() const;

	// 开启描边，支持传入不同的颜色数值 (比如青色 251) 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat Interface")
	void HighlightActor(int32 StencilValue);

	// 关闭描边   无敌高亮效果
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat Interface")
	void UnHighlightActor();

	// 新增：让 GA 来索要蒙太奇的接口
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat Interface")
	UAnimMontage* GetCombatMontageByTag(const FGameplayTag& MontageTag);

	// 获取战斗插槽的位置 (例如武器尖端、左手、右手，用来生成特效或进行伤害判定)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat Interface")
	FVector GetCombatSocketLocation(const FGameplayTag& MontageTag);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateFacingTarget(const FVector& Target);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetHitReactMontage();

	virtual void Die() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat Interface")
	TArray<FTaFeiTaggedMontage> GetAttackMontages();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	ETaFeiCharacterClass GetCharacterClass();

	
};
