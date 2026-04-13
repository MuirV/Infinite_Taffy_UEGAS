// Copyright Daisy


#include "AI/BTTaskPerformAttack.h"
#include "AIController.h"
#include "Character/EnemyBase.h"


UBTTaskPerformAttack::UBTTaskPerformAttack()
{
	NodeName = "Perform Attack";
}

EBTNodeResult::Type UBTTaskPerformAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		AEnemyBase* Enemy = Cast<AEnemyBase>(AIController->GetPawn());
		if (Enemy)
		{
			
			Enemy->PerformAttack();
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}
