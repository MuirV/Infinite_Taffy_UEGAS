// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TaFeiPlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UTaFeiPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GAMEANIMATIONSAMPLE_API ITaFeiPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 属性加点相关
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TaFei|PlayerInterface")
	int32 GetAttributePoints() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TaFei|PlayerInterface")
	int32 GetSpellPoints() const;

	// 经验与等级相关 (可根据你的需求后续在 PlayerState 中实现)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TaFei|PlayerInterface")
	int32 GetXP() const;
};
