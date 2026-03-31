// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "CombatMontageData.generated.h"



class UAnimMontage;
// 定义一个结构体，把 Tag 和 蒙太奇 绑在一起
USTRUCT(BlueprintType)
struct FTaFeiMontageInfo
{
	GENERATED_BODY()

	// 比如填入: Abilities.Attack.ComboLMB
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TaFei|Combat")
	FGameplayTag MontageTag;

	// 对应的蒙太奇
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TaFei|Combat")
	TObjectPtr<UAnimMontage> Montage;
};

/**
 * 战斗数据资产：存放角色或武器的所有攻击蒙太奇
 */
UCLASS(BlueprintType)
class GAMEANIMATIONSAMPLE_API UCombatMontageData : public UDataAsset
{
	GENERATED_BODY()
public:
	// 在蓝图里填写的连招蒙太奇列表
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TaFei|Combat")
	TArray<FTaFeiMontageInfo> CombatMontages;

	// 提供一个辅助函数，方便以后用 C++ 或蓝图直接根据 Tag 找蒙太奇
	UFUNCTION(BlueprintCallable, Category = "TaFei|Combat")
	UAnimMontage* GetMontageByTag(const FGameplayTag& Tag) const;
};
