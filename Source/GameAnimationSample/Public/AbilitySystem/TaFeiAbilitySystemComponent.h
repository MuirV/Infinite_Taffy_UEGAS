// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "TaFeiAbilitySystemComponent.generated.h"


class UCharacterClassInfo;
enum class ETaFeiCharacterClass : uint8;

class UTaFeiAbilitySystemComponent;

// 声明我们自定义的多播委托，用于向 UI 发送资产标签
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilitiesGiven, UTaFeiAbilitySystemComponent*); // 技能赋予完毕委托
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);
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
	FAbilitiesGiven AbilitiesGivenDelegate; // 新增：UI 监听这个委托来初始化技能图标


	//这里Aura用的是：AddCharacterAbilities,我们这里让PlayerState职责减轻，都放在ASC中干活
	void AddStartupAbilitiesFromData(const UCharacterClassInfo* CharacterData, ETaFeiCharacterClass CharacterClass, int32 Level);
			
	bool bStartupAbilitiesGiven = false; 

	
	void ForEachAbility(const FForEachAbility& Delegate);

	
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);//P239
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);

	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server, Reliable)
	void ServerUpdateAttribute(const FGameplayTag& AttributeTag);
	
	// 自定义的 Tag 输入处理函数
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
protected:

	// 监听无敌 Tag 变化的回调
	virtual void OnInvincibleTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	

	virtual void OnRep_ActivateAbilities() override;
	
	// 客户端 RPC：当 GE 应用给自己时触发。必须是 Client，因为 UI 只有客户端有！
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);
};
