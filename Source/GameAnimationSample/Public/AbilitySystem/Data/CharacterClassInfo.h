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

	// 专属主属性 (比如 Boss 的力量肯定比 Mob 高)
	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;

	// 专属初始技能 (比如 Player 有连招，Boss 有特定的砸地技能)
	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TArray<FTaFeiAbilityInfo> StartupAbilities;

	// 击杀该类型敌人奖励的经验值 (配置曲线，随等级提升)
	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	FScalableFloat XPReward = FScalableFloat();
};

UCLASS(BlueprintType)
class GAMEANIMATIONSAMPLE_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	// ★ 核心字典：根据枚举配置不同的主属性和技能
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TMap<ETaFeiCharacterClass, FTaFeiCharacterClassDefaultInfo> CharacterClassInformation;

	// 公共副属性 (所有角色通用，比如最大生命值 = 力量 * 10)
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;

	// 公共核心属性 (所有角色通用，用来填满当前血量和蓝量)
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	// 公共技能 (所有角色通用，比如 HitReact 受击反应技能)
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;

	// 辅助函数：根据枚举获取对应的结构体数据
	UFUNCTION(BlueprintCallable, Category="TaFei|CharacterClassData")
	FTaFeiCharacterClassDefaultInfo GetClassDefaultInfo(ETaFeiCharacterClass CharacterClass) const;
	
};
