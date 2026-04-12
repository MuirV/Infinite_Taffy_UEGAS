

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/TaFeiDamageGameplayAbility.h"
#include "TaFeiUltimateGameplayAbility.generated.h"

UCLASS()
class GAMEANIMATIONSAMPLE_API UTaFeiUltimateGameplayAbility : public UTaFeiDamageGameplayAbility
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "TaFei|Ultimate")
	void CauseUltimateDamage(AActor* TargetActor, float ChargeTime);

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TaFei|Ultimate")
	float MaxChargeMultiplier = 1.5f;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TaFei|Ultimate")
	float MaxChargeTime = 2.0f;
};