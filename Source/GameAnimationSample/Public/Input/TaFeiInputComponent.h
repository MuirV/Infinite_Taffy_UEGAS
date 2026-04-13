// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "TaFeiInputConfig.h"
#include "TaFeiInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API UTaFeiInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
public:
	// 使用模板函数，方便将输入绑定到任意对象（比如我们的 PlayerController）的方法上
	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
	void BindAbilityActions(const UTaFeiInputConfig* InputConfig,
		UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc);
};

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UTaFeiInputComponent::BindAbilityActions(const UTaFeiInputConfig* InputConfig, UserClass* Object,
	PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);

	for (const FTaFeiInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			
			if (PressedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}

			
			if (HeldFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Action.InputTag);
			}

		
			if (ReleasedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
			}
		}
	}
}