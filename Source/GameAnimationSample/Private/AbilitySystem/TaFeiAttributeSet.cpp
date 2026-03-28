// Copyright Daisy


#include "AbilitySystem/TaFeiAttributeSet.h"

UTaFeiAttributeSet::UTaFeiAttributeSet()
{
	// 给个默认初始值，方便测试
	InitHealth(100.f);
	InitMaxHealth(100.f);
	InitMana(200.f);
	InitMaxMana(200.f);
	InitUltimateEnergy(0.f);
	InitMaxUltimateEnergy(100.f);
}

void UTaFeiAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// 限制血量在 0 到 MaxHealth 之间
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
	// 限制能量在 0 到 MaxUltimateEnergy 之间
	else if (Attribute == GetUltimateEnergyAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxUltimateEnergy());
	}
	
}