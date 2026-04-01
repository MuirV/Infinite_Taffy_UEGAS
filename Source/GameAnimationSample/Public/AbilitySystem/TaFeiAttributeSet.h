// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "TaFeiAttributeSet.generated.h"

// 生成 Getter 和 Setter 的宏
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

// ★ 引入 Aura 的上下文信息结构体，方便我们在计算伤害时获取攻击者和受击者的所有信息
USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()
	
	FEffectProperties(){}
	
	FGameplayEffectContextHandle EffectContextHandle;
	
	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;
	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;
	UPROPERTY()
	AController* SourceController = nullptr;
	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;
	
	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;
	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;
	UPROPERTY()
	AController* TargetController = nullptr;
	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
};

// ★ 供 UI 使用的函数指针映射，允许我们通过 Tag 查找对应的属性获取函数
template<class T>
using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;

UCLASS()
class GAMEANIMATIONSAMPLE_API UTaFeiAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UTaFeiAttributeSet();
	
	// 网络复制必须重写的函数
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; 

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// ★ 映射表：存放 GameplayTag 和 属性Getter函数 的绑定关系
	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributes;

	/*
	 * Vital Attributes (生命值、法力值、大招能量)
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Attributes|Vital")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UTaFeiAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Attributes|Vital")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UTaFeiAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Attributes|Vital")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UTaFeiAttributeSet, Mana);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Attributes|Vital")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UTaFeiAttributeSet, MaxMana);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_UltimateEnergy, Category = "Attributes|Vital")
	FGameplayAttributeData UltimateEnergy;
	ATTRIBUTE_ACCESSORS(UTaFeiAttributeSet, UltimateEnergy);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxUltimateEnergy, Category = "Attributes|Vital")
	FGameplayAttributeData MaxUltimateEnergy;
	ATTRIBUTE_ACCESSORS(UTaFeiAttributeSet, MaxUltimateEnergy);

	/*
	 * Combat Attributes (战斗系数)
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DamageMultiplier, Category = "Attributes|Combat")
	FGameplayAttributeData DamageMultiplier;
	ATTRIBUTE_ACCESSORS(UTaFeiAttributeSet, DamageMultiplier);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DamageReduction, Category = "Attributes|Combat")
	FGameplayAttributeData DamageReduction;
	ATTRIBUTE_ACCESSORS(UTaFeiAttributeSet, DamageReduction);

	/*
	 * Meta Attributes (元属性，不参与网络同步)
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Meta")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UTaFeiAttributeSet, IncomingDamage);

	// ---------------- 网络同步回调函数 ----------------
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;
	UFUNCTION()
	void OnRep_UltimateEnergy(const FGameplayAttributeData& OldEnergy) const;
	UFUNCTION()
	void OnRep_MaxUltimateEnergy(const FGameplayAttributeData& OldMaxEnergy) const;
	UFUNCTION()
	void OnRep_DamageMultiplier(const FGameplayAttributeData& OldMultiplier) const;
	UFUNCTION()
	void OnRep_DamageReduction(const FGameplayAttributeData& OldReduction) const;

private:
	// 内部辅助函数：用来在应用 GE 后解包各种对象信息
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
};