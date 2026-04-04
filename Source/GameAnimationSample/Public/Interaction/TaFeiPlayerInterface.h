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

	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TaFei|PlayerInterface")
	int32 FindLevelForXP(int32 InXP) const;
	
	// 属性加点相关
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TaFei|PlayerInterface")
	int32 GetAttributePoints(int32 Level) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TaFei|PlayerInterface")
	int32 GetSpellPoints(int32 Level) const;

	// 经验与等级相关 (可根据你的需求后续在 PlayerState 中实现)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TaFei|PlayerInterface")
	int32 GetXP() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TaFei|PlayerInterface")
	void AddToPlayerLevel(int32 InPlayerLevel);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TaFei|PlayerInterface")
	void AddToAttributePoints(int32 InAttributePoints);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TaFei|PlayerInterface")
	void AddToSpellPoints(int32 InSpellPoints);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TaFei|PlayerInterface")
	void AddToXP(int32 InXP);

	// 触发升级时的表现 (比如升级特效、满血满蓝)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TaFei|PlayerInterface")
	void LevelUp();
	
};
