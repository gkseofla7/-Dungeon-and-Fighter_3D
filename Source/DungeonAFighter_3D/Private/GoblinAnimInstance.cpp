// Fill out your copyright notice in the Description page of Project Settings.


#include "GoblinAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "DFGhostKnight.h"
UGoblinAnimInstance::UGoblinAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM(TEXT("AnimMontage'/Game/Animations/Goblin_Skeleton_Montage.Goblin_Skeleton_Montage'"));
	if (AM.Succeeded())
	{
		AttackMontage = AM.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> IM(TEXT("AnimMontage'/Game/Animations/Gobliin_Impact_Montage.Gobliin_Impact_Montage'"));
	if (IM.Succeeded())
	{
		ImpactMontage = IM.Object;
	}
	IsDead = false;
}
void UGoblinAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	//매 프레임마다 한번씩 마지막에 호출됨
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();//나를 가지고 있는 Pawn 호출, 2개이상이면..?

	if (!::IsValid(Pawn))
		return;
	if (!IsDead)
	{
		Speed = Pawn->GetVelocity().Size();

		auto Character = Cast<ADFGhostKnight>(Pawn);
		if (Character)
		{
			IsFalling = Character->GetMovementComponent()->IsFalling();

			Vertical = Character->LeftRightValue;
			Horizontal = Character->UpDownValue;
		}

	}

}

void UGoblinAnimInstance::PlayAttackMontage()
{

	Montage_Play(AttackMontage, 1.f);

}
void UGoblinAnimInstance::PlayImpactMontage()
{
	Montage_Play(ImpactMontage, 1.f);
}

void UGoblinAnimInstance::AnimNotify_AttackHit()
{
	//UE_LOG(LogTemp, Log, TEXT("AnimNotify_AttackHit"));
	OnAttackHit.Broadcast();
}

void UGoblinAnimInstance::AnimNotify_OnAnimEnd_Impact()
{
	OnImpactHit.Broadcast();
}