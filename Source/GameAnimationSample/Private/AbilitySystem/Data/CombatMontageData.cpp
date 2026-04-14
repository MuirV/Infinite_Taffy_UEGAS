// Copyright Daisy


#include "AbilitySystem/Data/CombatMontageData.h"

TArray<UAnimMontage*> UCombatMontageData::GetMontagesByTag(const FGameplayTag& Tag) const
{
	for (const FTaFeiMontageInfo& Info : CombatMontages)
	{
		if (Info.MontageTag == Tag)
		{
			// 返回这套连招的所有 Montage
			return Info.Montages;
		}
	}
	return TArray<UAnimMontage*>();
}