// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TaFeiAbilitySystemLibrary.generated.h"

class UTaFeiOverlayWidgetController;
class UTaFeiAttributeMenuWidgetController;

UCLASS()
class GAMEANIMATIONSAMPLE_API UTaFeiAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// ================= UI 获取工具 =================
	
	UFUNCTION(BlueprintPure, Category="TaFei|UIWidgetController", meta=(DefaultToSelf="WorldContextObject"))
	static UTaFeiOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category="TaFei|UIWidgetController", meta=(DefaultToSelf="WorldContextObject"))
	static UTaFeiAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	// ================= 战斗与阵营工具 =================
	
	// 判断两个 Actor 是否为敌对关系 (通过 Tag 区分 Player 和 Enemy)
	UFUNCTION(BlueprintPure, Category="TaFei|Combat")
	static bool IsNotFriend(AActor* FirstActor, AActor* SecondActor);

	// 获取指定半径内存活的玩家/敌人 (用于 AOE 范围伤害)
	UFUNCTION(BlueprintCallable, Category="TaFei|Combat", meta=(DefaultToSelf="WorldContextObject"))
	static void GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin);
};