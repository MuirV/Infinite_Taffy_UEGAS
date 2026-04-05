// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ActiveGameplayEffectHandle.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "WaitCooldownChange.generated.h"

struct FGameplayEffectSpec;
class UAbilitySystemComponent;

// 定义冷却改变时的多播委托（输出剩余时间）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTaFeiCooldownChangeSignature, float, TimeRemaining);

/**
 * 监听 GAS 技能冷却标签变化的异步蓝图节点
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = "AsyncTask"))
class GAMEANIMATIONSAMPLE_API UWaitCooldownChange : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	// 冷却开始时触发
	UPROPERTY(BlueprintAssignable)
	FTaFeiCooldownChangeSignature CooldownStart;

	// 冷却结束时触发
	UPROPERTY(BlueprintAssignable)
	FTaFeiCooldownChangeSignature CooldownEnd;

	// 实例化异步节点的静态工厂函数
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UWaitCooldownChange* WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& InCooldownTag);

	// 结束并销毁任务
	UFUNCTION(BlueprintCallable)
	void EndTask();

protected:

	// 存储绑定的 ASC
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
    
	FGameplayTag CooldownTag;

	// 回调函数：当冷却 Tag 的数量发生变化时
	void CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount);
	
	// 回调函数：当有新的 GameplayEffect 应用到身上时
	void OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle);
};