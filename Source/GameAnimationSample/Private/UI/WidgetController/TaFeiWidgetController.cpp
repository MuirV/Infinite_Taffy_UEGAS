// Copyright Daisy

#include "UI/WidgetController/TaFeiWidgetController.h"
#include "Player/TaFeiPlayerController.h"
#include "Player/TaFeiPlayerState.h"
#include "AbilitySystem/TaFeiAbilitySystemComponent.h"
#include "AbilitySystem/TaFeiAttributeSet.h"

void UTaFeiWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UTaFeiWidgetController::BroadcastInitialValues() {}
void UTaFeiWidgetController::BindCallbacksToDependencies() {}

ATaFeiPlayerController* UTaFeiWidgetController::GetTaFeiPC()
{
	if (TaFeiPlayerController == nullptr)
	{
		TaFeiPlayerController = Cast<ATaFeiPlayerController>(PlayerController);
	}
	return TaFeiPlayerController;
}

ATaFeiPlayerState* UTaFeiWidgetController::GetTaFeiPS()
{
	if (TaFeiPlayerState == nullptr)
	{
		TaFeiPlayerState = Cast<ATaFeiPlayerState>(PlayerState);
	}
	return TaFeiPlayerState;
}

UTaFeiAbilitySystemComponent* UTaFeiWidgetController::GetTaFeiASC()
{
	if (TaFeiAbilitySystemComponent == nullptr)
	{
		TaFeiAbilitySystemComponent = Cast<UTaFeiAbilitySystemComponent>(AbilitySystemComponent);
	}
	return TaFeiAbilitySystemComponent;
}

UTaFeiAttributeSet* UTaFeiWidgetController::GetTaFeiAS()
{
	if (TaFeiAttributeSet == nullptr)
	{
		TaFeiAttributeSet = Cast<UTaFeiAttributeSet>(AttributeSet);
	}
	return TaFeiAttributeSet;
}