// Copyright Daisy

#include "UI/WidgetController/TaFeiOverlayWidgetController.h"
#include "AbilitySystem/TaFeiAbilitySystemComponent.h"
#include "AbilitySystem/TaFeiAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
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
	ATaFeiPlayerState* TaFeiPS = Cast<ATaFeiPlayerState>(PlayerState);
	if (TaFeiPS)
	{
		// 绑定经验值变化
		TaFeiPS->OnXPChangedDelegate.AddUObject(this, &UTaFeiOverlayWidgetController::OnXPChanged);
        
		// 新增：绑定等级变化！
		TaFeiPS->OnLevelChangedDelegate.AddLambda(
			[this](int32 NewLevel)
			{
				// 向 UI 蓝图广播新的等级数值！
				OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
			}
		);
	}
 	
	// 绑定核心生命、法力、大招属性的变化监听
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

	

	//  绑定技能与屏幕消息
	if (GetTaFeiASC())
	{
		// --- 修复：绑定初始技能的监听，防止 UI 没反应 ---
		if (GetTaFeiASC()->bStartupAbilitiesGiven)
		{
			OnInitializeStartupAbilities(GetTaFeiASC());
		}
		else
		{
			GetTaFeiASC()->AbilitiesGivenDelegate.AddUObject(this, &UTaFeiOverlayWidgetController::OnInitializeStartupAbilities);
		}

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
// 修复：形参必须改成 TaFeiASC，不要遗留 Aura 的名字
void UTaFeiOverlayWidgetController::OnInitializeStartupAbilities(UTaFeiAbilitySystemComponent* TaFeiASC)
{
	if (!TaFeiASC->bStartupAbilitiesGiven) return;

	FForEachAbility BroadcastDelegate;
    
	// 修复：Lambda 捕获此处的 TaFeiASC
	BroadcastDelegate.BindLambda([this, TaFeiASC](const FGameplayAbilitySpec& AbilitySpec)
	{
	   //获取该技能的 Tag
	   FGameplayTag AbilityTag = TaFeiASC->GetAbilityTagFromSpec(AbilitySpec);
       
	   // 从你配置好的 AbilityInfo 中查找 UI 信息
	   FGAAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
       
	   // 获取该技能当前绑定的输入 Tag
	   Info.InputTag = TaFeiASC->GetInputTagFromSpec(AbilitySpec);
       
	   //  广播给蓝图 UI
	   AbilityInfoDelegate.Broadcast(Info);
	});
    
	TaFeiASC->ForEachAbility(BroadcastDelegate);
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
	ATaFeiPlayerState* TaFeiPS = CastChecked<ATaFeiPlayerState>(PlayerState);
	const ULevelUpInfo* LevelUpInfo = TaFeiPS->LevelUpInfo;

	checkf(LevelUpInfo, TEXT("Unable to find LevelUpInfo, Please fill out TaFeiPlayerState Blueprint"));

	const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num() - 1;

	if (Level <= MaxLevel && Level > 0)
	{
		// 本级升级所需总经验
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
		// 上一级升级所需总经验
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level - 1].LevelUpRequirement;

		// 本级总共需要打多少经验
		const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
		// 当前在本级已经打到了多少经验
		const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;
	
		// 计算百分比
		float XPBarPercent = 0.f;
		if (DeltaLevelRequirement > 0)
		{
			XPBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelRequirement);
		}

		OnXPPercentageChangedDelegate.Broadcast(XPBarPercent);
	}
}