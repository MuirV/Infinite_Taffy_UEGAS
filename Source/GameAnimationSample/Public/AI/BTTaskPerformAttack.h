// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskPerformAttack.generated.h"

/**
 * 
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API UBTTaskPerformAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTaskPerformAttack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;	
};
