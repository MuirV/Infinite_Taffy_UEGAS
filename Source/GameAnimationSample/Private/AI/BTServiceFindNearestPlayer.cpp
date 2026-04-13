// Copyright Daisy


#include "AI/BTServiceFindNearestPlayer.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"


void UBTServiceFindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();
	
	
	TArray<AActor*> FoundPlayers;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Player"), FoundPlayers);

	float ClosestDistance = MAX_FLT;
	AActor* ClosestPlayer = nullptr;

	for (AActor* Player : FoundPlayers)
	{
		float Distance = OwningPawn->GetDistanceTo(Player);
		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestPlayer = Player;
		}
	}

	if (ClosestPlayer)
	{
		
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetActorKey.SelectedKeyName, ClosestPlayer);
    
		
		float Distance = OwningPawn->GetDistanceTo(ClosestPlayer);
		OwnerComp.GetBlackboardComponent()->SetValueAsFloat(FName("DistanceToTarget"), Distance);
	}
}