// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Data/CharacterClassInfo.h"
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


	
	static int32 GetXPRewardForClassAndLevel(const UObject* WorldContextObject, ETaFeiCharacterClass CharacterClass, int32 CharacterLevel);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ETaFeiCharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void GiveStartupGameplayAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ETaFeiCharacterClass CharacterClass);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);

	// ================= 战斗与阵营工具 =================
	
	// 判断两个 Actor 是否为敌对关系 (通过 Tag 区分 Player 和 Enemy)
	UFUNCTION(BlueprintPure, Category="TaFei|Combat")
	static bool IsNotFriend(AActor* FirstActor, AActor* SecondActor);

	// 获取指定半径内存活的玩家/敌人 (用于 AOE 范围伤害)
	UFUNCTION(BlueprintCallable, Category="TaFei|Combat", meta=(DefaultToSelf="WorldContextObject"))
	static void GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin);
};