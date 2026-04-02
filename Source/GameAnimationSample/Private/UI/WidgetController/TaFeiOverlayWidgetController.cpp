// Copyright Daisy

#include "UI/WidgetController/TaFeiOverlayWidgetController.h"
#include "AbilitySystem/TaFeiAbilitySystemComponent.h"
#include "AbilitySystem/TaFeiAttributeSet.h"
#include "Player/TaFeiPlayerState.h"

void UTaFeiOverlayWidgetController::BroadcastInitialValues()
{
	// 广播血量、蓝量、大招的初始值
	OnHealthChanged.Broadcast(GetTaFeiAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetTaFeiAS()->GetMaxHealth());
	
	OnManaChanged.Broadcast(GetTaFeiAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetTaFeiAS()->GetMaxMana());

	OnUltimateEnergyChanged.Broadcast(GetTaFeiAS()->GetUltimateEnergy());
	OnMaxUltimateEnergyChanged.Broadcast(GetTaFeiAS()->GetMaxUltimateEnergy());
}

void UTaFeiOverlayWidgetController::BindCallbacksToDependencies()
{
	// 1. 绑定核心生命、法力、大招属性的变化监听
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetTaFeiAS()->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnHealthChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetTaFeiAS()->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnMaxHealthChanged.Broadcast(Data.NewValue); }
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetTaFeiAS()->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnManaChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetTaFeiAS()->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnMaxManaChanged.Broadcast(Data.NewValue); }
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetTaFeiAS()->GetUltimateEnergyAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnUltimateEnergyChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetTaFeiAS()->GetMaxUltimateEnergyAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnMaxUltimateEnergyChanged.Broadcast(Data.NewValue); }
	);

	// 2. 绑定经验与等级监听 (需要在 TaFeiPlayerState 中有对应的委托)
	/* GetTaFeiPS()->OnXPChangedDelegate.AddUObject(this, &UTaFeiOverlayWidgetController::OnXPChanged);
	GetTaFeiPS()->OnLevelChangedDelegate.AddLambda(
		[this](int32 NewLevel)
		{
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel);		
		}
	);
	*/

	// 3. 绑定 Gameplay Effect 带来的屏幕消息 (拾取物品、提示等)
	if (GetTaFeiASC())
	{
		// 监听 ASC 广播过来的 EffectAssetTags
		GetTaFeiASC()->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				for (const FGameplayTag& Tag : AssetTags)
				{
					// 过滤出 "Message" 开头的标签 (例如 Message.HealthPotion)
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if (Tag.MatchesTag(MessageTag))
					{
						const FTaFeiUIWidgetRow* Row = GetDataTableRowByTag<FTaFeiUIWidgetRow>(MessageWidgetDataTable, Tag);
						if (Row)
						{
							MessageWidgetRowDelegate.Broadcast(*Row);
						}
					}
				}
			}
		);
	}
}

// 预留的 XP 转换逻辑，等你以后做 LevelUpInfo 数据资产时可以在这里写逻辑
void UTaFeiOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	/* * 参照 Aura 的逻辑：
	 * 获取 LevelUpInfo，找到当前 XP 对应的 Level，
	 * 然后计算 (NewXP - 上一级所需XP) / (这一级所需XP - 上一级所需XP)
	 * 算出百分比后广播：
	 * OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
	 */
}