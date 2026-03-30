// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h" // 必须引入Tag头文件
#include "CharacterClassData.generated.h"

class UGameplayAbility;
class UGameplayEffect;
/**
 * 
 */
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

UCLASS(BlueprintType)
class GAMEANIMATIONSAMPLE_API UCharacterClassData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	// 初始属性（GE）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> PrimaryAttributes;

	// 将原来的 TArray<TSubclassOf...> 改为我们自定义的结构体数组
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FTaFeiAbilityInfo> StartupAbilities;
	
};
