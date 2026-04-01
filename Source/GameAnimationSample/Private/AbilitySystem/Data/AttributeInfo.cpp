// Copyright Daisy

#include "AbilitySystem/Data/AttributeInfo.h"

FTaFeiAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	for (const FTaFeiAttributeInfo& Info : AttributeInformation)
	{
		if (Info.AttributeTag == AttributeTag)
		{
			return Info;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("找不到属性 Tag [%s] 的信息，请检查 AttributeInfo 数据资产: [%s]"), *AttributeTag.ToString(), *GetNameSafe(this));
	}

	return FTaFeiAttributeInfo();
}