// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TaFeiWidgetController.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class ATaFeiPlayerController;
class ATaFeiPlayerState;
class UTaFeiAbilitySystemComponent;
class UTaFeiAttributeSet;

USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()
	FWidgetControllerParams() {}
	FWidgetControllerParams(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
		: PlayerController(PC), PlayerState(PS), AbilitySystemComponent(ASC), AttributeSet(AS) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;
};

UCLASS()
class GAMEANIMATIONSAMPLE_API UTaFeiWidgetController : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);

	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues();
	virtual void BindCallbacksToDependencies();

protected:
	// 基础指针
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<APlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UAttributeSet> AttributeSet;

	// === TaFei 专属缓存指针与 Getter ===
	UPROPERTY()
	TObjectPtr<ATaFeiPlayerController> TaFeiPlayerController;
	UPROPERTY()
	TObjectPtr<ATaFeiPlayerState> TaFeiPlayerState;
	UPROPERTY()
	TObjectPtr<UTaFeiAbilitySystemComponent> TaFeiAbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UTaFeiAttributeSet> TaFeiAttributeSet;

	ATaFeiPlayerController* GetTaFeiPC();
	ATaFeiPlayerState* GetTaFeiPS();
	UTaFeiAbilitySystemComponent* GetTaFeiASC();
	UTaFeiAttributeSet* GetTaFeiAS();
};