// Fill out your copyright notice in the Description page of Project Settings.


#include "GoblinAIController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
const FName AGoblinAIController::HomePosKey(TEXT("HomePos"));
const FName AGoblinAIController::PatrolPosKey(TEXT("PatrolPos"));
const FName AGoblinAIController::TargetKey(TEXT("Target"));
const FName AGoblinAIController::IsAttackedKey(TEXT("IsAttacked"));
const FName AGoblinAIController::DamagedPosKey(TEXT("DamagedPos"));
AGoblinAIController::AGoblinAIController()
{

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BT(TEXT("BehaviorTree'/Game/AI/BT_Goblin.BT_Goblin'"));
	if (BT.Succeeded())
	{
		BehaviorTree = BT.Object;
	}
	static ConstructorHelpers::FObjectFinder <UBlackboardData> BD(TEXT("BlackboardData'/Game/AI/BB_Goblin.BB_Goblin'"));
	if (BD.Succeeded())
	{
		BlackboardData = BD.Object;
	}


}
void AGoblinAIController::OnPossess(APawn* InPawn)
{
	
	Super::OnPossess(InPawn);
	
	//GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGoblinAIController::RandomMove, 3.f, true);
	
	if (UseBlackboard(BlackboardData, Blackboard))
	{
		Blackboard->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());
		if (RunBehaviorTree(BehaviorTree))
		{
			
		}
	}
	
}

void AGoblinAIController::OnUnPossess()
{
	Super::OnUnPossess();

	//GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void AGoblinAIController::RandomMove()
{
	auto CurrentPawn = GetPawn();
	if (CurrentPawn == nullptr)
		return;
	
	
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (NavSystem==nullptr)
	{
		
		return;
	}
	


	FNavLocation NextLocation;

	if (NavSystem->GetRandomPointInNavigableRadius(FVector(1300.0f,0.f,0.f), 500.0f, NextLocation))
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, NextLocation.Location);
	}
}
