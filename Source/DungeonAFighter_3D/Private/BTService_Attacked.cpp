// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Attacked.h"
#include "GoblinAIController.h"
#include "DFGhostKnight.h"
#include "DFGKAnimInstance.h"
#include "DFGoblin.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
UBTService_Attacked::UBTService_Attacked()
{
	NodeName = TEXT("Attacked");
	Interval = 1.0f;
}

void UBTService_Attacked::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);


	auto ABCharacter = Cast<ADFGoblin>(OwnerComp.GetAIOwner()->GetPawn());
	auto Target = Cast<ADFGhostKnight>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AGoblinAIController::TargetKey));
	if (Target == nullptr)
		return;
	FVector OposDir = ABCharacter->GetActorLocation() - Target->GetActorLocation();
	OposDir = FVector(-100.0f, -100.0f, -100.0f) * UKismetMathLibrary::Normal(OposDir);


	OwnerComp.GetBlackboardComponent()->SetValueAsBool(AGoblinAIController::IsAttackedKey, ABCharacter->IsAttacked);
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(AGoblinAIController::DamagedPosKey,  OposDir);
	
}
