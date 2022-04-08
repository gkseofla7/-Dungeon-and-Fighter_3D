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
	IsDead = false;
}
void UGoblinAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	//�� �����Ӹ��� �ѹ��� �������� ȣ���
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();//���� ������ �ִ� Pawn ȣ��, 2���̻��̸�..?

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

void UGoblinAnimInstance::AnimNotify_AttackHit()
{
	//UE_LOG(LogTemp, Log, TEXT("AnimNotify_AttackHit"));
	OnAttackHit.Broadcast();
}

