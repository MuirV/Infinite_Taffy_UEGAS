// Copyright Daisy


#include "Character/TaFeiCharacterBase.h"
#include "AbilitySystem/TaFeiAbilitySystemComponent.h"
#include "AbilitySystem/TaFeiAttributeSet.h"

// 已弃用，蓝图层面使用GASP BP_SandboxCharacter作为父类，采用PlayerState传递ASC，AS等
ATaFeiCharacterBase::ATaFeiCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	// 实例化 ASC 和 AS
	AbilitySystemComponent = CreateDefaultSubobject<UTaFeiAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UTaFeiAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* ATaFeiCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Called when the game starts or when spawned
void ATaFeiCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}


