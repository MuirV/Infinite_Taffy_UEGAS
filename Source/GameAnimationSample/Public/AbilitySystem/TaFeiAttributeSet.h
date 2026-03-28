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
	
};
