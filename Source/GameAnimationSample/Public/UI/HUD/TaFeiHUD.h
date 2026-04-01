#pragma once
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TaFeiHUD.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UTaFeiUserWidget;
class UTaFeiOverlayWidgetController;
class UTaFeiAttributeMenuWidgetController;
struct FWidgetControllerParams;

UCLASS()
class GAMEANIMATIONSAMPLE_API ATaFeiHUD : public AHUD
{
	GENERATED_BODY()
public:
	// 主界面 Overlay (血条、蓝条、大招能量)
	UTaFeiOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
	// 属性面板
	UTaFeiAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

private:
	UPROPERTY()
	TObjectPtr<UTaFeiUserWidget> OverlayWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UTaFeiUserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UTaFeiOverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UTaFeiOverlayWidgetController> OverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UTaFeiAttributeMenuWidgetController> AttributeMenuWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UTaFeiAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;
};