// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Damaged.h"
#include "GoblinAIController.h"
#include "DFGhostKnight.h"
#include "DFGoblin.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_Damaged::UBTTask_Damaged()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Damaged::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto ABCharacter = Cast<ADFGoblin>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ABCharacter)
		return EBTNodeResult::Failed;
	auto Target = Cast<ADFGhostKnight>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AGoblinAIController::TargetKey));
	FVector OposDir = ABCharacter->GetActorLocation() - Target->GetActorLocation();
	OposDir = FVector(-1000.0f, -1000.0f, -1000.0f) * UKismetMathLibrary::Normal(OposDir);
	ABCharacter->LaunchCharacter(OposDir, false, false);



	UE_LOG(LogTemp, Log, TEXT("???? %f %f %f"), OposDir.X, OposDir.Y, OposDir.Z);

	return EBTNodeResult::InProgress;
}

void UBTTask_Damaged::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	auto ABCharacter = Cast<ADFGoblin>(OwnerComp.GetAIOwner()->GetPawn());
	if (ABCharacter->IsAttacked == false)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

}