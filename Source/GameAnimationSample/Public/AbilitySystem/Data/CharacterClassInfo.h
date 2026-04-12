// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h" // 必须引入Tag头文件
#include "ScalableFloat.h" //写经验值，每种类型的经验值掉落
#include "CharacterClassInfo.generated.h"

class UGameplayAbility;
class UGameplayEffect;
/**
 * 
 */

// 新增角色职业/类型枚举
UENUM(BlueprintType)
enum class ETaFeiCharacterClass : uint8
{
	Player,     // 玩家控制的 TaFei
	Mob,        // 小兵、爪牙
	Boss        // 关卡首领
};

// Input config, InputComponent 之后 ：将技能类和输入标签绑定在一起的结构体
USTRUCT(BlueprintType)
struct FTaFeiAbilityInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag InputTag;
};

// 特定职业专属的初始数据
USTRUCT(BlueprintType)
struct FTaFeiCharacterClassDefaultInfo
{
	GENERATED_BODY()


	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;


	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TArray<FTaFeiAbilityInfo> StartupAbilities;

	
	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	FScalableFloat XPReward = FScalableFloat();
};

UCLASS(BlueprintType)
class GAMEANIMATIONSAMPLE_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	

	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TMap<ETaFeiCharacterClass, FTaFeiCharacterClassDefaultInfo> CharacterClassInformation;

	
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;

	
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;

	UFUNCTION(BlueprintCallable, Category="TaFei|CharacterClassData")
	FTaFeiCharacterClassDefaultInfo GetClassDefaultInfo(ETaFeiCharacterClass CharacterClass) const;
	
};
