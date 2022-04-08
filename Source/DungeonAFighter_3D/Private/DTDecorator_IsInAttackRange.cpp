// Fill out your copyright notice in the Description page of Project Settings.


#include "DTDecorator_IsInAttackRange.h"
#include "GoblinAIController.h"
#include "DFGhostKnight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DFGKAnimInstance.h"

UDTDecorator_IsInAttackRange::UDTDecorator_IsInAttackRange()
{
	NodeName = TEXT("CanAttack");
}


bool UDTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
		return false;

	auto Target = Cast<ADFGhostKnight>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AGoblinAIController::TargetKey));
	if (nullptr == Target)
		return false;
	if (Target->getAnim()->GetDead() == true)//Á×¾úÀ¸¸é
		return false;

	bResult = (Target->GetDistanceTo(ControllingPawn) <= 200.0f);
	return bResult;
}