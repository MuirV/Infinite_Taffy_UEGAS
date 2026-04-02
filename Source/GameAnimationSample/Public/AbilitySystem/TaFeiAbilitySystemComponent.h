// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "TaFeiAbilitySystemComponent.generated.h"

// ★声明我们自定义的多播委托，用于向 UI 发送资产标签
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/);
/**
 * 
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API UTaFeiAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	UTaFeiAbilitySystemComponent();

	void AbilityActorInfoSet();
	
	FEffectAssetTags EffectAssetTags;
	// 自定义的 Tag 输入处理函数
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
protected:
	// 客户端 RPC：当 GE 应用给自己时触发。必须是 Client，因为 UI 只有客户端有！
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);
};
