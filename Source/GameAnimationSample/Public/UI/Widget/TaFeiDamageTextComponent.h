#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "TaFeiDamageTextComponent.generated.h"

UCLASS()
class GAMEANIMATIONSAMPLE_API UTaFeiDamageTextComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDamageText(float Damage, bool bBlockedHit, bool bCriticalHit);
};