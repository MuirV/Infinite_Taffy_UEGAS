// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h" // 必须包含这个才能用 FGameplayTag
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "TaFeiCombatInterface.generated.h"


USTRUCT(BlueprintType)
struct FTaFeiTaggedMontage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* Montage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FGameplayTag MontageTag; // 比如：Abilities.Attack.ComboLMB

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag SocketTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* ImpactSound = nullptr;
};

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
	
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat Interface")
	int32 GetPlayerLevel() const;

	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat Interface")
	void HighlightActor(int32 StencilValue);

	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat Interface")
	void UnHighlightActor();

	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat Interface")
	TArray<UAnimMontage*> GetCombatMontagesByTag(const FGameplayTag& MontageTag);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat Interface")
	FVector GetCombatSocketLocation(const FGameplayTag& MontageTag);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateFacingTarget(const FVector& Target);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetHitReactMontage();

	virtual void Die() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat Interface")
	TArray<FTaFeiTaggedMontage> GetAttackMontages();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	ETaFeiCharacterClass GetCharacterClass();

	
};
