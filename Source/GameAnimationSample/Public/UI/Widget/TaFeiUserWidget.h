#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TaFeiUserWidget.generated.h"

UCLASS()
class GAMEANIMATIONSAMPLE_API UTaFeiUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
};