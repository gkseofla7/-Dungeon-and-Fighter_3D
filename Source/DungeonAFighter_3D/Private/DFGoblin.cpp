// Fill out your copyright notice in the Description page of Project Settings.


#include "DFGoblin.h"


#include "DFGhostKnight.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GoblinAnimInstance.h"
#include "DrawDebugHelpers.h"
#include "StatComponent.h"
#include "Components/WidgetComponent.h"
#include "DFGKWidget.h"
#include "DFWeapon.h"
#include "Item.h"
#include "InventoryComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DFGameHudWidget.h"
#include "GoblinAIController.h"
#include "ABCharacterSetting.h"
#include "DFGameInstance.h"
// Sets default values

// Sets default values
ADFGoblin::ADFGoblin()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM(TEXT("SkeletalMesh'/Game/Goblin/goblin_d_shareyko.goblin_d_shareyko'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> RM(TEXT("Material'/Game/phong1_inst.phong1_inst'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> BRM(TEXT("Material'/Game/Goblin/phong1.phong1'"));

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	if (RM.Succeeded())
	{
		RedStoredMaterial = RM.Object;

			

	}
	if (BRM.Succeeded())
	{
		StoredMaterial = BRM.Object;
	}
	if (SM.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SM.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("AnimBlueprint'/Game/Blueprints/ABP_GoblinAnim.ABP_GoblinAnim_C'"));

	if (WARRIOR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}
	//Don't rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.f, 0.f, -78.f), FRotator(0.f, -90.f, 0.f)
	);
	GetMesh()->SetRelativeScale3D(FVector(0.6f, 0.6f, 0.6f));
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("GhostKnight"));

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Face in the direction we are moving.. 왜 true로 돼있지
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.f;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->MaxFlySpeed = 600.f;
	
	Stat = CreateDefaultSubobject<UStatComponent>(TEXT("GoblinSTAT"));

	HpBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBAR"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetWidgetSpace(EWidgetSpace::Screen);//2D형식
	HpBar->SetRelativeLocation(FVector(0.f, 0.f, 200.f));

	static ConstructorHelpers::FClassFinder<UDFGKWidget>UW(TEXT("WidgetBlueprint'/Game/UI/WBP_HpBar.WBP_HpBar_C'"));
	if (UW.Succeeded())
	{
		HpBar->SetWidgetClass(UW.Class);
		HpBar->SetDrawSize(FVector2D(200.f, 50.f));
	}

	AIControllerClass = AGoblinAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	


}

void ADFGoblin::PostInitializeComponents()
{
	Super::PostInitializeComponents();


	AnimInstance = Cast<UGoblinAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &ADFGoblin::OnAttackMontageEnded);
		AnimInstance->OnAttackHit.AddUObject(this, &ADFGoblin::AttackCheck);
	}
	HpBar->InitWidget();//이걸 꼭 추가
	//Delegate 바인딩 해주는 부분
	auto HpWidget = Cast<UDFGKWidget>(HpBar->GetUserWidgetObject());
	if (HpWidget)
		HpWidget->BindHp(Stat);

}
// Called when the game starts or when spawned
void ADFGoblin::BeginPlay()
{
	Super::BeginPlay();
	

	time = GetWorld()->GetTimeSeconds();



}

void ADFGoblin::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	IsAttacking = false;
	OnAttackEnd.Broadcast();
}

// Called every frame
void ADFGoblin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADFGoblin::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// set up gameplay key bindings
	//PlayerInputComponent->BindAction(TEXT("Inventory"), this, &ADFGoblin::Inventory);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &ADFGoblin::Attack);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ADFGoblin::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("MoveUp"), this, &ADFGoblin::MoveUp);


}

void ADFGoblin::DrinkHp(float HpAmount)
{
	Stat->DrinkHp(-HpAmount);

}
float ADFGoblin::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Stat->OnAttacked(DamageAmount);
	if (Stat->GetHp() == 0)
	{
		AnimInstance->SetDeadAnim();
		SetActorEnableCollision(false);
	}
	ChangeDamageColor();

	return DamageAmount;
}
void ADFGoblin::DrinkMp(float MpAmount)
{
	Stat->DrinkMp(-MpAmount);

}


bool ADFGoblin::CanSetWeapon()
{
	return (nullptr == CurrentWeapon);
}

void ADFGoblin::SetWeapon(ADFWeapon* NewWeapon)
{
	FName WeaponSocket(TEXT("hand_l_socket"));
	if (nullptr != NewWeapon)
	{
		NewWeapon->AttachToComponent(GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;
	}
}

void ADFGoblin::ChangeDamageColor()
{



	FTimerHandle WaitHandle;
	float WaitTime = 0.2; //시간을 설정하고
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			GetMesh()->SetMaterial(0, StoredMaterial);
			UE_LOG(LogTemp, Log, TEXT("Back"));


			
		}), WaitTime, false); //반복도 여기서 추가 변수를 선언해 설정가능

	

	GetMesh()->SetMaterial(0, RedStoredMaterial);
	UE_LOG(LogTemp, Log, TEXT("Red"));
	
	
}



void ADFGoblin::MoveRight(float Value)
{

	LeftRightValue = Value;
	if (Value != 0)
		GetController()->SetControlRotation(FRotationMatrix::MakeFromX(FVector(0, -Value, 0)).Rotator());

	AddMovementInput(FVector(0.f, -1.f, 0.f), Value);
}

void ADFGoblin::MoveUp(float Value)
{
	UpDownValue = Value;
	if (GetController()->GetControlRotation().Yaw == 90)
		UpDownValue = -Value;
	AddMovementInput(FVector(1.f, 0.f, 0.f), Value);


}


void ADFGoblin::Attack()
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
	//AnimInstance->JumpToSection(AttackIndex);


	AttackIndex = (AttackIndex + 1) % 3;
	IsAttacking = true;
}

void ADFGoblin::AttackCheck()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);

	float AttackRange = 100.f;
	float AttackRadius = 50.f;


	bool bResult = GetWorld()->SweepSingleByChannel(
		OUT HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(AttackRadius),
		Params);
	FVector Vec = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + Vec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat Rotation = FRotationMatrix::MakeFromZ(Vec).ToQuat();
	FColor DrawColor;
	if (bResult)
		DrawColor = FColor::Green;
	else
		DrawColor = FColor::Red;


	DrawDebugCapsule(GetWorld(), Center, HalfHeight, AttackRadius,
		Rotation, DrawColor, false, 5.f);
	if (bResult && HitResult.Actor.IsValid())
	{
		//UE_LOG(LogTemp, Log, TEXT("Hit Actor : %s"), *HitResult.Actor->GetName());
		FDamageEvent DamageEvent;
		//피해자
		HitResult.Actor->TakeDamage(Stat->GetAttack(), DamageEvent, GetController(), this);
	}
}

void ADFGoblin::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// jump on any touch
	Jump();
}

void ADFGoblin::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	StopJumping();
}

float ADFGoblin::GetHpPercent() { return Stat->GetHpRatio(); }
float ADFGoblin::GetMpPercent() { return 1.0; }

void ADFGoblin::DisableMouseCursor() const
{
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetShowMouseCursor(false);
}



