// Fill out your copyright notice in the Description page of Project Settings.


#include "DFGhostKnight.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DFGKAnimInstance.h"
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
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

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

	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	Inventory->Capacity = 20;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Face in the direction we are moving.. 왜 true로 돼있지
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MaxFlySpeed = 600.f;

	Stat = CreateDefaultSubobject<UStatComponent>(TEXT("STAT"));

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
	/// <summary>
	/// 
	/// </summary>
	//GameHudReference = CreateDefaultSubobject<UDFGameHudWidget>(TEXT("GameHud"));


	//static ConstructorHelpers::FClassFinder<UDFGKWidget>GHR(TEXT("WidgetBlueprint'/Game/UI/GameHud.GameHud_C'"));
	//if (GHR.Succeeded())
	//{
	//	GameHudReference->SetWidgetClass(GHR.Class);

	//}
	//AIControllerClass = AGoblinAIController::StaticClass();
	//AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("GhostKnight"));
	auto DefaultSetting = GetDefault<UABCharacterSetting>();
	if (DefaultSetting->CharacterAssets.Num() > 0)
	{
		for (auto CharacterAssets : DefaultSetting->CharacterAssets)
		{
			UE_LOG(LogTemp, Log, TEXT("Character Asset : %s"), *CharacterAssets.ToString());
		}
	}

}
void ADFGhostKnight::PostInitializeComponents()
{
	Super::PostInitializeComponents();


	AnimInstance = Cast<UDFGKAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &ADFGhostKnight::OnAttackMontageEnded);
		AnimInstance->OnAttackHit.AddUObject(this, &ADFGhostKnight::AttackCheck);
	}
	HpBar->InitWidget();//이걸 꼭 추가
	//Delegate 바인딩 해주는 부분
	auto HpWidget = Cast<UDFGKWidget>(HpBar->GetUserWidgetObject());
	if (HpWidget)
		HpWidget->BindHp(Stat);

}
// Called when the game starts or when spawned
void ADFGhostKnight::BeginPlay()
{
	Super::BeginPlay();

	/*FName WeaponSocket(TEXT("hand_l_socket"));
	auto CurWeapon = GetWorld()->SpawnActor<ADFWeapon>(FVector::ZeroVector, FRotator::ZeroRotator);
	
	if (CurWeapon != nullptr)
	{
		CurWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
			WeaponSocket);
	}*/
	time = GetWorld()->GetTimeSeconds();

	if (!IsPlayerControlled())
	{
		auto DefaultSetting = GetDefault<UABCharacterSetting>();
		int32 RandIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);
		CharacterAssetToLoad = DefaultSetting->CharacterAssets[RandIndex];

		auto ABGameInstance = Cast<UDFGameInstance>(GetGameInstance());
		if (nullptr != ABGameInstance)
		{
			AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(
				CharacterAssetToLoad, FStreamableDelegate::CreateUObject(this, &ADFGhostKnight::OnAssetLoadCompleted));//그 후 실행

		}

	}
	
}

void ADFGhostKnight::OnAssetLoadCompleted()
{
	USkeletalMesh* AssetLoaded = Cast<USkeletalMesh>(AssetStreamingHandle->GetLoadedAsset());
	AssetStreamingHandle.Reset();
	if (nullptr != AssetLoaded)
	{
		GetMesh()->SetSkeletalMesh(AssetLoaded);
	}
}
void ADFGhostKnight::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	IsAttacking = false;
	OnAttackEnd.Broadcast();
}

// Called every frame
void ADFGhostKnight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//GetController()->SetControlRotation(FRotationMatrix::MakeFromX(FVector(0.f, rot, 0.f)).Rotator());
	
}

// Called to bind functionality to input
void ADFGhostKnight::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// set up gameplay key bindings
	//PlayerInputComponent->BindAction(TEXT("Inventory"), this, &ADFGhostKnight::Inventory);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &ADFGhostKnight::Attack);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ADFGhostKnight::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("MoveUp"), this, &ADFGhostKnight::MoveUp);
	

}

void ADFGhostKnight::DrinkHp(float HpAmount)
{
	Stat->DrinkHp(-HpAmount);

}
float ADFGhostKnight::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Stat->OnAttacked(DamageAmount);
	if (Stat->GetHp() == 0)
	{
		AnimInstance->SetDeadAnim();
		SetActorEnableCollision(false);
	}

	
	return DamageAmount;
}
void ADFGhostKnight::DrinkMp(float MpAmount)
{
	Stat->DrinkMp(-MpAmount);

}


bool ADFGhostKnight::CanSetWeapon()
{
	return (nullptr == CurrentWeapon);
}

void ADFGhostKnight::SetWeapon(ADFWeapon* NewWeapon)
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

void ADFGhostKnight::UseItem(UItem* Item)
{
	if (Item)
	{
		Item->Use(this);
		//Item->OnUse(this);
	}
}

void ADFGhostKnight::MoveRight(float Value)
{
	// add movement in that direction
	//if (Value == 0)
	//	return;

	LeftRightValue = Value;
	if(Value!=0)
		GetController()->SetControlRotation(FRotationMatrix::MakeFromX(FVector(0, -Value, 0)).Rotator());
	//if (Value > 0)
	//	rot = 180.0;
	//else if (Value < 0)
	//	rot = 0.;


	
	//AddActorLocalRotation(FRotator(0.0f, _rot, 0.f));
	//GetMesh()->SetRelativeRotation(FRotator(0.f, rot, 0.f));
	AddMovementInput(FVector(0.f, -1.f, 0.f), Value);
}

void ADFGhostKnight::MoveUp(float Value)
{
	UpDownValue = Value;
	if(GetController()->GetControlRotation().Yaw ==90)
		UpDownValue = -Value;
	AddMovementInput(FVector(1.f, 0.f, 0.f), Value);
		

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
	UE_LOG(LogTemp, Log, TEXT("Hit"));
	time = GetWorld()->GetTimeSeconds();
	AnimInstance->PlayAttackMontage();
	AnimInstance->JumpToSection(AttackIndex);

	
	AttackIndex = (AttackIndex + 1) % 3;
	IsAttacking = true;
}

void ADFGhostKnight::AttackCheck()
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

	UE_LOG(LogTemp, Log, TEXT("Hit Actor"));
	DrawDebugCapsule(GetWorld(), Center, HalfHeight, AttackRadius,
		Rotation, DrawColor, false, 5.f);
	if (bResult && HitResult.Actor.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("Hit Actor : %s"), *HitResult.Actor->GetName());
		FDamageEvent DamageEvent;
		//피해자
		HitResult.Actor->TakeDamage(Stat->GetAttack(), DamageEvent, GetController(), this);
	}
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

float ADFGhostKnight::GetHpPercent() { return Stat->GetHpRatio(); }
float ADFGhostKnight::GetMpPercent() { return 1.0;}

void ADFGhostKnight::DisableMouseCursor() const
{
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetShowMouseCursor(false);
}
