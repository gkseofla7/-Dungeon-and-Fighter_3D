// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "DTDecorator_IsInAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONAFIGHTER_3D_API UDTDecorator_IsInAttackRange : public UBTDecorator
{
	GENERATED_BODY()

public:
	UDTDecorator_IsInAttackRange();


protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory) const override;
	
};
