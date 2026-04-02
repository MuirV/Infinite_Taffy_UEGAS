

#include "UI/WidgetController/TaFeiOverlayWidgetController.h"
#include "AbilitySystem/TaFeiAttributeSet.h"
#include "AbilitySystemComponent.h"

void UTaFeiOverlayWidgetController::BroadcastInitialValues()
{
	const UTaFeiAttributeSet* TaFeiAS = CastChecked<UTaFeiAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(TaFeiAS->GetHealth());
	OnMaxHealthChanged.Broadcast(TaFeiAS->GetMaxHealth());
	OnManaChanged.Broadcast(TaFeiAS->GetMana());
	OnMaxManaChanged.Broadcast(TaFeiAS->GetMaxMana());
	OnUltimateEnergyChanged.Broadcast(TaFeiAS->GetUltimateEnergy());
	OnMaxUltimateEnergyChanged.Broadcast(TaFeiAS->GetMaxUltimateEnergy());
}

void UTaFeiOverlayWidgetController::BindCallbacksToDependencies()
{
	const UTaFeiAttributeSet* TaFeiAS = CastChecked<UTaFeiAttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(TaFeiAS->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnHealthChanged.Broadcast(Data.NewValue); });

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(TaFeiAS->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnMaxHealthChanged.Broadcast(Data.NewValue); });

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(TaFeiAS->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnManaChanged.Broadcast(Data.NewValue); });

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(TaFeiAS->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnMaxManaChanged.Broadcast(Data.NewValue); });

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(TaFeiAS->GetUltimateEnergyAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnUltimateEnergyChanged.Broadcast(Data.NewValue); });

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(TaFeiAS->GetMaxUltimateEnergyAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnMaxUltimateEnergyChanged.Broadcast(Data.NewValue); });
}