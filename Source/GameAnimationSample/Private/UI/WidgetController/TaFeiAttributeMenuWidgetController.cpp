#include "UI/WidgetController/TaFeiAttributeMenuWidgetController.h"
#include "AbilitySystem/TaFeiAttributeSet.h"
#include "GameFramework/PlayerState.h"
#include "Interaction/TaFeiPlayerInterface.h"
#include "UI/HUD/TaFeiHUD.h"

void UTaFeiAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	check(AttributeInfo); // 确保蓝图里挂载了 DataAsset

	// 1. 遍历并绑定所有我们在 C++ 里存入 TagsToAttributes 的属性
	for (auto& Pair : GetTaFeiAS()->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}

	// 2. 绑定属性点变化 (通过肉体 Pawn 调用接口，完美解耦)
	if (APawn* AvatarPawn = PlayerController->GetPawn())
	{
		if (AvatarPawn->Implements<UTaFeiPlayerInterface>())
		{
			// TODO: 如果你在 PlayerState 里加了 OnAttributePointsChangedDelegate，可以在这里绑定
			// GetTaFeiPS()->OnAttributePointsChangedDelegate.AddLambda(...)
		}
	}
}

void UTaFeiAttributeMenuWidgetController::BroadcastInitialValues()
{
	check(AttributeInfo); // 确保蓝图里配了 DataAsset
	
	// GetTaFeiAS() 是你写在基类里的辅助函数，用来安全获取 AttributeSet
	for (auto& Pair : GetTaFeiAS()->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}

	// 广播当前的属性点
	if (PlayerState && PlayerState->Implements<UTaFeiPlayerInterface>())
	{
		// 现在的 Execute_GetAttributePoints 只需要传入目标对象 (PlayerState) 即可，完美契合 1 个参数！
		int32 Points = ITaFeiPlayerInterface::Execute_GetAttributePoints(PlayerState);
		AttributePointsChangedDelegate.Broadcast(Points);
	}
}

void UTaFeiAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	// 去 DataAsset 里查找配置好的名字和描述
	FTaFeiAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	
	// 从 AttributeSet 里实时提取该属性当前的数值
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	
	// 将打包好的数据广播给蓝图 UI
	AttributeInfoDelegate.Broadcast(Info);
}