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


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TaFei|Combat")
	FGameplayTag MontageTag;


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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TaFei|Combat")
	TArray<FTaFeiMontageInfo> CombatMontages;

	
	UFUNCTION(BlueprintCallable, Category = "TaFei|Combat")
	UAnimMontage* GetMontageByTag(const FGameplayTag& Tag) const;
};
