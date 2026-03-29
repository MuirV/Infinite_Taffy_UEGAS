// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassData.generated.h"

class UGameplayAbility;
class UGameplayEffect;
/**
 * 
 */
UCLASS(BlueprintType)
class GAMEANIMATIONSAMPLE_API UCharacterClassData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	// 初始属性（GE）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> PrimaryAttributes;

	// 初始技能
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	
};
