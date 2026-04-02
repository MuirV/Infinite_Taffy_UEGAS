#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/TaFeiWidgetController.h"
#include "AbilitySystem/Data/AttributeInfo.h" // 你的数据资产结构体
#include "TaFeiAttributeMenuWidgetController.generated.h"

struct FGameplayAttribute;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FTaFeiAttributeInfo&, Info);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributePointsChangedSignature, int32, Points);

UCLASS(BlueprintType, Blueprintable)
class GAMEANIMATIONSAMPLE_API UTaFeiAttributeMenuWidgetController : public UTaFeiWidgetController
{
	GENERATED_BODY()
public:
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;


	// ★ 挂载属性详情的数据资产
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UAttributeInfo> AttributeInfo;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;
	// 监听剩余属性点
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FAttributePointsChangedSignature AttributePointsChangedDelegate;
	
	
private:
	void BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const;
	
};