// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GoblinAIController.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONAFIGHTER_3D_API AGoblinAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AGoblinAIController();
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	static const FName HomePosKey;
	static const FName PatrolPosKey;
	static const FName TargetKey;
private:
	void RandomMove();

private:
	FTimerHandle TimerHandle;

	UPROPERTY()
	class UBehaviorTree* BehaviorTree;

	UPROPERTY()
	class UBlackboardData* BlackboardData;
};
