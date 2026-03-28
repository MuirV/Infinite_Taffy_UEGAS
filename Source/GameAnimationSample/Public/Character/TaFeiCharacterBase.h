// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "TaFeiCharacterBase.generated.h"


class UTaFeiAbilitySystemComponent;
class UTaFeiAttributeSet;

UCLASS()
class GAMEANIMATIONSAMPLE_API ATaFeiCharacterBase : public ACharacter, public IAbilitySystemInterface 
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATaFeiCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UTaFeiAttributeSet* GetAttributeSet() const {return AttributeSet;}
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// 声明 ASC 和 AS 指针 [cite: 566]
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	TObjectPtr<UTaFeiAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	TObjectPtr<UTaFeiAttributeSet> AttributeSet;
	
};
