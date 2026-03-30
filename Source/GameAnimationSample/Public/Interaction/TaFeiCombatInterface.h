// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TaFeiCombatInterface.generated.h"

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
};
