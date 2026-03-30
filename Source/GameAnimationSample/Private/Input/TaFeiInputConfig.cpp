// Copyright Daisy


#include "Input/TaFeiInputConfig.h"

const UInputAction* UTaFeiInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FTaFeiInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("找不到 InputTag [%s] 对应的 InputAction，请检查 InputConfig 数据资产。"), *InputTag.ToString());
	}

	return nullptr;
}

