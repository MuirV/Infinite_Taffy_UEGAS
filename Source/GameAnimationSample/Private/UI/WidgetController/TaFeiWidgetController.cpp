

#include "UI/WidgetController/TaFeiWidgetController.h"

void UTaFeiWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UTaFeiWidgetController::BroadcastInitialValues()
{
	
}

void UTaFeiWidgetController::BindCallbacksToDependencies()
{
	
}