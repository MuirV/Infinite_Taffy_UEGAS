// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "TaFeiEffectActor.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;

// 添加 TaFei 前缀，防止枚举重名
UENUM(BlueprintType)
enum class ETaFeiEffectApplicationPolicy : uint8
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

UENUM(BlueprintType)
enum class ETaFeiEffectRemovalPolicy : uint8
{
	RemoveOnEndOverlap,
	DoNotRemove
};

UCLASS()
class GAMEANIMATIONSAMPLE_API ATaFeiEffectActor : public AActor
{
	GENERATED_BODY()
    
public: 
	ATaFeiEffectActor();

protected:
	virtual void BeginPlay() override;


	UFUNCTION(BlueprintCallable, Category = "TaFei|EffectActor")
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);

	// 蓝图碰撞组件触发时调用此函数
	UFUNCTION(BlueprintCallable, Category = "TaFei|EffectActor")
	void OnOverlap(AActor* TargetActor);

	// 蓝图碰撞组件结束触发时调用此函数
	UFUNCTION(BlueprintCallable, Category = "TaFei|EffectActor")
	void OnEndOverlap(AActor* TargetActor);

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	bool bDestroyOnEffectApplication = false;

	// 是否对敌人也生效 (比如毒沼泽可以毒怪物，但血瓶通常只给玩家吃)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	bool bApplyEffectsToEnemies = false;

	// 施加效果时的默认等级
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	float ActorLevel = 1.f;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects|Instant")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects|Instant")
	ETaFeiEffectApplicationPolicy InstantEffectApplicationPolicy = ETaFeiEffectApplicationPolicy::DoNotApply;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects|Duration")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects|Duration")
	ETaFeiEffectApplicationPolicy DurationEffectApplicationPolicy = ETaFeiEffectApplicationPolicy::DoNotApply;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects|Infinite")
	TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects|Infinite")
	ETaFeiEffectApplicationPolicy InfiniteEffectApplicationPolicy = ETaFeiEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects|Infinite")
	ETaFeiEffectRemovalPolicy InfiniteEffectRemovalPolicy = ETaFeiEffectRemovalPolicy::RemoveOnEndOverlap;

	// 记录无限效果的 Handle，以便在离开区域时移除
	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;
};