// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "GoblinAIController.h"
#include "DFGhostKnight.h"
#include "DFGoblin.h"

UBTTask_Attack::UBTTask_Attack()
{
	bNotifyTick = true;
	IsAttacking = false;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	
	auto ABCharacter = Cast<ADFGoblin>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ABCharacter)
		return EBTNodeResult::Failed;

	ABCharacter->Attack();
	IsAttacking = true;
	ABCharacter->OnAttackEnd.AddLambda([this]() -> void {
		UE_LOG(LogTemp, Log, TEXT("AttackEnd"));
		IsAttacking = false;
		});
	return EBTNodeResult::InProgress;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	auto ABCharacter = Cast<ADFGoblin>(OwnerComp.GetAIOwner()->GetPawn());
	IsAttacking = ABCharacter->IsAttacking;
	if (!IsAttacking)
	{
		UE_LOG(LogTemp, Log, TEXT("Finished"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

}