

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/TaFeiDamageGameplayAbility.h"
#include "TaFeiUltimateGameplayAbility.generated.h"

UCLASS()
class GAMEANIMATIONSAMPLE_API UTaFeiUltimateGameplayAbility : public UTaFeiDamageGameplayAbility
{
	GENERATED_BODY()

public:
	// 暴露给蓝图：在松开按键，且动画打到敌人身上时调用
	UFUNCTION(BlueprintCallable, Category = "TaFei|Ultimate")
	void CauseUltimateDamage(AActor* TargetActor, float ChargeTime);

protected:
	// 蓄力最多带来的额外增伤倍率 (比如最多额外增加 50% 伤害)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TaFei|Ultimate")
	float MaxChargeMultiplier = 1.5f;

	// 达到最大增伤需要的蓄力时间 (秒)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TaFei|Ultimate")
	float MaxChargeTime = 2.0f;
};