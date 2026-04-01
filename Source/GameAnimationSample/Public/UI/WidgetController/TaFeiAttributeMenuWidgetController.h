#pragma once
#include "CoreMinimal.h"
#include "UI/WidgetController/TaFeiWidgetController.h"
#include "TaFeiAttributeMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributePointsChangedSignature, int32, Points);

UCLASS(BlueprintType, Blueprintable)
class GAMEANIMATIONSAMPLE_API UTaFeiAttributeMenuWidgetController : public UTaFeiWidgetController
{
	GENERATED_BODY()
public:
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;

	// 监听剩余属性点
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FAttributePointsChangedSignature AttributePointsChangedDelegate;
	
	// TODO: 等你迁移 Aura 的 AttributeInfo 数据资产后，可在这里扩展广播具体的属性值
};