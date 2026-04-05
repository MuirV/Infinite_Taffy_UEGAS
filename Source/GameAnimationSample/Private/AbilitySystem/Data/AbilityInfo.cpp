// Copyright Daisy


#include "AbilitySystem/Data/AbilityInfo.h"

FGAAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (const FGAAbilityInfo& Info : AbilityInformation)
	{
		if (Info.AbilityTag == AbilityTag)
		{
			return Info;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("在AbilityInfo [%s] 中找不到信息给AbilityTag [%s] "), *GetNameSafe(this), *AbilityTag.ToString());
		
	}
	return FGAAbilityInfo();
}
