// Copyright Daisy


#include "AbilitySystem/Data/TaFeiCombatMontageData.h"

UAnimMontage* UTaFeiCombatMontageData::GetMontageByTag(const FGameplayTag& Tag) const
{
	// 遍历数组，找到 Tag 匹配的蒙太奇并返回
	for (const FTaFeiMontageInfo& Info : CombatMontages)
	{
		if (Info.MontageTag == Tag)
		{
			return Info.Montage;
		}
	}
	
	return nullptr;
}