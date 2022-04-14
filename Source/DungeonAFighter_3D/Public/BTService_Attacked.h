// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Attacked.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONAFIGHTER_3D_API UBTService_Attacked : public UBTService
{
	GENERATED_BODY()
public:
	UBTService_Attacked();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
		float DeltaSeconds) override;
};
