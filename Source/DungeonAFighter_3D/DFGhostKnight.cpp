// Fill out your copyright notice in the Description page of Project Settings.


#include "DFGhostKnight.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DFGKAnimInstance.h"
// Sets default values
ADFGhostKnight::ADFGhostKnight()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM(TEXT("SkeletalMesh'/Game/ParagonGreystone/Characters/Heroes/Greystone/Meshes/Greystone.Greystone'"));

	if (SM.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SM.Object);
	}
	 //Don't rotate when the controller rotates.
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	//CapsuleComponent가 RootComponent

	CameraBoom->SetUsingAbsoluteRotation(true); // Rotation of the character should not affect rotation of boom
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->TargetArmLength = 750.f;
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 75.f);
	CameraBoom->SetRelativeRotation(FRotator(-25.f, 180.f, 0.f));

	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.f, 0.f, -88.f), FRotator(0.f, 0.f, 0.f)
	);

	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	SideViewCameraComponent->bUsePawnControlRotation = false; // We don't want the controller rotating the camera


	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Face in the direction we are moving..
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MaxFlySpeed = 600.f;

}

// Called when the game starts or when spawned
void ADFGhostKnight::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = Cast<UDFGKAnimInstance>(GetMesh()->GetAnimInstance());
	AnimInstance->OnMontageEnded.AddDynamic(this, &ADFGhostKnight::OnAttackMontageEnded);
	time = GetWorld()->GetTimeSeconds();
	
}

void ADFGhostKnight::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	IsAttacking = false;
}

// Called every frame
void ADFGhostKnight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

// Called to bind functionality to input
void ADFGhostKnight::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// set up gameplay key bindings
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &ADFGhostKnight::Attack);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ADFGhostKnight::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("MoveUp"), this, &ADFGhostKnight::MoveUp);

}

void ADFGhostKnight::MoveRight(float Value)
{
	// add movement in that direction
	float rot = 0;
	//if (Value == 0)
	//	return;
	LeftRightValue = Value;
	if (Value >= 0)
		rot = 0.;
	else
		rot = 180.;

	GetMesh()->SetRelativeRotation(FRotator(0.f, rot, 0.f));
	AddMovementInput(FVector(0.f, -1.f, 0.f), Value);
}

void ADFGhostKnight::MoveUp(float Value)
{
	UpDownValue = Value;
	AddMovementInput(FVector(-1.f, 0.f, 0.f), Value);
	
	
}


void ADFGhostKnight::Attack()
{
	if (IsAttacking)
		return;
	
	float CurTime = GetWorld()->GetTimeSeconds() - time;
	
	if (CurTime > 1.)//1초 넘어가면
	{
		AttackIndex = 0;
	}
	
	time = GetWorld()->GetTimeSeconds();
	AnimInstance->PlayAttackMontage();
	AnimInstance->JumpToSection(AttackIndex);

	
	AttackIndex = (AttackIndex + 1) % 3;
	IsAttacking = true;
}

void ADFGhostKnight::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// jump on any touch
	Jump();
}

void ADFGhostKnight::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	StopJumping();
}
