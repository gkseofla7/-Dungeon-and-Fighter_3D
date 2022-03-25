// Fill out your copyright notice in the Description page of Project Settings.


#include "DFGKAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "DFGhostKnight.h"
UDFGKAnimInstance::UDFGKAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM(TEXT("AnimMontage'/Game/Animations/Greystone_Skeleton_Montage.Greystone_Skeleton_Montage'"));
	if (AM.Succeeded())
	{
		AttackMontage = AM.Object;
	}
}
void UDFGKAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	//매 프레임마다 한번씩 마지막에 호출됨
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();//나를 가지고 있는 Pawn 호출, 2개이상이면..?
	if (IsValid(Pawn))
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

void UDFGKAnimInstance::PlayAttackMontage()
{

	Montage_Play(AttackMontage, 1.f);
	
}

void UDFGKAnimInstance::JumpToSection(int32 SectionIndex)
{
	FName Name = GetAttackMontageName(SectionIndex);
	Montage_JumpToSection(Name, AttackMontage);
}

FName UDFGKAnimInstance::GetAttackMontageName(int32 SectionIndex)
{
	UE_LOG(LogTemp, Log, TEXT("Attack%d"), SectionIndex);
	return FName(FString::Printf(TEXT("Attack%d"), SectionIndex));

	//FName??
}

void UDFGKAnimInstance::AnimNotify_AttackHit()
{
	//UE_LOG(LogTemp, Log, TEXT("AnimNotify_AttackHit"));
	OnAttackHit.Broadcast();
}
