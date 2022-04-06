// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPatrolPos.h"
#include "GoblinAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
	NodeName = TEXT("FindPatrolPos");
}

EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto CurrentPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (CurrentPawn == nullptr)
		return EBTNodeResult::Failed;


	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(CurrentPawn->GetWorld());
	if (NavSystem == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(AGoblinAIController::HomePosKey);
	FNavLocation NextLocation;

	if (NavSystem->GetRandomPointInNavigableRadius(Origin, 500.0f, NextLocation))
	{
		//UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, NextLocation.Location);
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(FName(TEXT("PatrolPos")), NextLocation.Location);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;

}