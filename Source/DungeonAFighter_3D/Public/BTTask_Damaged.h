// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Damaged.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONAFIGHTER_3D_API UBTTask_Damaged : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_Damaged();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
private:

};
