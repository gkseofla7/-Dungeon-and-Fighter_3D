// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Detect.h"
#include "GoblinAIController.h"
#include "DFGhostKnight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");
	Interval = 1.0f;
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return;

	//UWolrd* World = ControllingPawn->GetWorld();
	FVector Center = ControllingPawn->GetActorLocation();
	float DetectRadius = 300.0f;

	//if (nullptr == World) return;
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);
	bool bResult = ControllingPawn->GetWorld()->OverlapMultiByChannel(
		OverlapResults, Center, FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParam
	);

	if (bResult)
	{
		for (auto const& OverlapResult : OverlapResults)
		{
			ADFGhostKnight* GhostKnight = Cast<ADFGhostKnight>(OverlapResult.GetActor());
			if (GhostKnight && GhostKnight->GetController()->IsPlayerController())
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(AGoblinAIController::TargetKey, GhostKnight);
				DrawDebugSphere(ControllingPawn->GetWorld(), Center, DetectRadius, 16, FColor::Green,
					false, 0.2f);
				DrawDebugPoint(ControllingPawn->GetWorld(), GhostKnight->GetActorLocation(), 10.0f,
					FColor::Blue, false, 0.2f);
				DrawDebugLine(ControllingPawn->GetWorld(), ControllingPawn->GetActorLocation(),
					GhostKnight->GetActorLocation(), FColor::Blue, false, 0.2f);
				return;
			}
		}
	}
	DrawDebugSphere(ControllingPawn->GetWorld(), Center, DetectRadius, 16, FColor::Red, false, 0.2f);
}