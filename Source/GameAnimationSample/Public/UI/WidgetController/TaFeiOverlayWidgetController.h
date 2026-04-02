// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/TaFeiWidgetController.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "TaFeiOverlayWidgetController.generated.h"

class UTaFeiUserWidget;

//  直接在这里定义弹窗消息的数据表结构 
USTRUCT(BlueprintType)
struct FTaFeiUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UTaFeiUserWidget> MessageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image = nullptr;
};

// --- 委托声明 ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, FTaFeiUIWidgetRow, Row);

/**
 * 主界面 UI 控制器
 */
UCLASS(BlueprintType, Blueprintable)
class GAMEANIMATIONSAMPLE_API UTaFeiOverlayWidgetController : public UTaFeiWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	// === 属性条 UI 委托 ===
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnMaxManaChanged;

	// ★ TaFei 专属：大招能量 UI 委托
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnUltimateEnergyChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnMaxUltimateEnergyChanged;

	// === 屏幕消息委托 ===
	UPROPERTY(BlueprintAssignable, Category="GAS|Messages")
	FMessageWidgetRowSignature MessageWidgetRowDelegate;

	// === 经验与等级委托 (预留拓展位) ===
	UPROPERTY(BlueprintAssignable, Category="GAS|XP")
	FOnAttributeChangedSignature OnXPPercentChangedDelegate; // 进度条百分比

	UPROPERTY(BlueprintAssignable, Category="GAS|Level")
	FOnPlayerStatChangedSignature OnPlayerLevelChangedDelegate;

protected:
	// ★ 挂载屏幕提示消息的数据表
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;

	// 辅助函数：根据 Tag 从 DataTable 提取消息数据
	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);

	// 等你以后加入了 LevelUpInfo 数据资产，可以在这里处理 XP 百分比转换
	void OnXPChanged(int32 NewXP);
};

// 模板函数实现必须放在头文件
template <typename T>
T* UTaFeiOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}