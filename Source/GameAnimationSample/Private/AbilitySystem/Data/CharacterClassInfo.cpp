// Copyright Daisy


#include "AbilitySystem/Data/CharacterClassInfo.h"


FTaFeiCharacterClassDefaultInfo UCharacterClassInfo::GetClassDefaultInfo(ETaFeiCharacterClass CharacterClass) const
{
	// 查找 Map 中是否配置了对应的职业数据
	if (const FTaFeiCharacterClassDefaultInfo* Info = CharacterClassInformation.Find(CharacterClass))
	{
		return *Info;
	}

	// 如果策划忘记配置了，打印个红字警告
	UE_LOG(LogTemp, Error, TEXT("缺少职业 [%d] 的配置数据！请检查 CharacterClassInfo 数据资产。"), static_cast<int32>(CharacterClass));
	
	return FTaFeiCharacterClassDefaultInfo();
}