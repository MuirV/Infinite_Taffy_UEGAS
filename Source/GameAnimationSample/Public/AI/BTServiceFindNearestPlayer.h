// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTServiceFindNearestPlayer.generated.h"

/**
 * 
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API UBTServiceFindNearestPlayer : public UBTService
{
	GENERATED_BODY()

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector TargetActorKey;	
	
};
