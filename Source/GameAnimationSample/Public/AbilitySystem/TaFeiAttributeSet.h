// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "TaFeiAttributeSet.generated.h"

/**
 * 
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class GAMEANIMATIONSAMPLE_API UTaFeiAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UTaFeiAttributeSet();

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Vital")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UTaFeiAttributeSet, Health);
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Vital")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UTaFeiAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Vital")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UTaFeiAttributeSet, Mana);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Vital")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UTaFeiAttributeSet, MaxMana);
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Vital")
	FGameplayAttributeData UltimateEnergy;
	ATTRIBUTE_ACCESSORS(UTaFeiAttributeSet, UltimateEnergy);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Vital")
	FGameplayAttributeData MaxUltimateEnergy;
	ATTRIBUTE_ACCESSORS(UTaFeiAttributeSet, MaxUltimateEnergy);
	
	// 伤害增幅倍率 (默认 1.0)
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat")
	FGameplayAttributeData DamageMultiplier;
	ATTRIBUTE_ACCESSORS(UTaFeiAttributeSet, DamageMultiplier);

	// 减伤比例 (默认 0.0，大招开启后可设为 0.9)
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat")
	FGameplayAttributeData DamageReduction;
	ATTRIBUTE_ACCESSORS(UTaFeiAttributeSet, DamageReduction);

	/* Meta Attributes */
	// 专门用于接收外界伤害数值的属性，不直接修改 Health，而是在
	// GameplayEffectExecutionCalculation 里根据 DamageMultiplier 和 DamageReduction 计算最终伤害后再修改 Health
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Meta")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UTaFeiAttributeSet, IncomingDamage);
	
};
