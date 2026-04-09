// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "TaFeiDamageActor.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;

/**
 * 专门用于造成 ExecCalc 环境伤害的陷阱/障碍物 Actor (如地刺、毒气)
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API ATaFeiDamageActor : public AActor
{
	GENERATED_BODY()
    
public: 
	ATaFeiDamageActor();

protected:
	// 蓝图碰撞组件触发时调用此函数
	UFUNCTION(BlueprintCallable, Category = "TaFei|DamageActor")
	void OnOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "TaFei|DamageActor")
	void OnEndOverlap(AActor* TargetActor);
	
	// 造成伤害的核心逻辑
	UFUNCTION(BlueprintCallable, Category = "TaFei|DamageActor")
	void ApplyBurning(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "TaFei|DamageActor")
	void RemoveBurning(AActor* TargetActor);

	// ==================== 伤害配置 ====================
    
	// 必须配置为写好的带 ExecCalc_Damage 的 GE (例如 GE_TestDamage)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage Settings")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// 伤害类型标签 (必须填入 Damage.Physical 或其他你在 ExecCalc 里读取的标签)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage Settings")
	FGameplayTag DamageTypeTag;

	// 基础伤害数值 (这个值会被打包进 SetByCaller 传给 ExecCalc)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage Settings")
	float BaseDamage = 20.f;

	// 陷阱的环境等级 (用于触发你的 ExecCalc 等级缩放计算)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage Settings")
	float TrapLevel = 1.f;

	// ==================== 属性配置 ====================

	// 是否踩上去触发一次就销毁 (例如一次性地雷选 True，常驻地刺选 False)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage Settings")
	bool bDestroyOnDamage = false;

	// 陷阱是否对敌人也造成伤害
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage Settings")
	bool bApplyDamageToEnemies = false;

	// 当前正在被燃烧的目标
	UPROPERTY()
	TMap<UAbilitySystemComponent*, FActiveGameplayEffectHandle> ActiveEffects;
};