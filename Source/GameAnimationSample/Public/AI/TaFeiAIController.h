// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TaFeiAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;
/**
 * 
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API ATaFeiAIController : public AAIController
{
	GENERATED_BODY()

public:
	ATaFeiAIController();

protected:
	
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
	
};
