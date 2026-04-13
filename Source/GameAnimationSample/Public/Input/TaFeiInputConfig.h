// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "TaFeiInputConfig.generated.h"

class UInputAction;
/**
 * 结构体：用于将 InputAction 与 GameplayTag 绑定
 */
USTRUCT(BlueprintType)
struct FTaFeiInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	const UInputAction* InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag();
};

/**
 * 数据资产：存放所有的输入绑定配置
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API UTaFeiInputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FTaFeiInputAction> AbilityInputActions;


	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = false) const;	
};
