#include "UI/WidgetController/TaFeiAttributeMenuWidgetController.h"
#include "AbilitySystem/TaFeiAttributeSet.h"
#include "GameFramework/PlayerState.h"
#include "Interaction/TaFeiPlayerInterface.h"
#include "UI/HUD/TaFeiHUD.h"

void UTaFeiAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	// 使用 Interface 从 PlayerState 身上获取点数，完美解耦！
	if (PlayerState->Implements<UTaFeiPlayerInterface>())
	{
		// TODO: 未来可以在 PlayerState 里添加一个 OnAttributePointsChanged 的多播委托
		// PlayerState->OnAttributePointsChanged.AddLambda(...)
	}
}

void UTaFeiAttributeMenuWidgetController::BroadcastInitialValues()
{
	if (PlayerState->Implements<UTaFeiPlayerInterface>())
	{
		int32 Points = ITaFeiPlayerInterface::Execute_GetAttributePoints(PlayerState);
		AttributePointsChangedDelegate.Broadcast(Points);
	}
}