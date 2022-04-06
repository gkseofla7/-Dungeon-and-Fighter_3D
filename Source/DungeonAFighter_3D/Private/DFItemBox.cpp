// Fill out your copyright notice in the Description page of Project Settings.


#include "DFItemBox.h"
#include "DFWeapon.h"
#include "DFGhostKnight.h"

// Sets default values
ADFItemBox::ADFItemBox()
{
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Box = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BOX"));
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EFFECT"));
	
	RootComponent = Trigger;
	Box->SetupAttachment(RootComponent);
	Effect->SetupAttachment(RootComponent);

	Trigger->SetBoxExtent(FVector(100.f, 100.f, 50.f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BOX(
		TEXT("StaticMesh'/Game/InfinityBladeGrassLands/Environments/Breakables/StaticMesh/Box/SM_Env_Breakables_Box1.SM_Env_Breakables_Box1'"));
	if (SM_BOX.Succeeded())
	{
		Box->SetStaticMesh(SM_BOX.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_CHESTOPEN(
		TEXT("ParticleSystem'/Game/InfinityBladeGrassLands/Effects/FX_Treasure/Chest/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh'")
	);
	if (P_CHESTOPEN.Succeeded())
	{
		Effect->SetTemplate(P_CHESTOPEN.Object);
		Effect->bAutoActivate = false;
	}
	Box->SetRelativeLocation(FVector(0.0f, -3.5f, -30.f));

	Trigger->SetCollisionProfileName(TEXT("ItemBox"));
	Box->SetCollisionProfileName(TEXT("NoCollision"));
	WeaponItemClass = ADFWeapon::StaticClass();
	
	
}

// Called when the game starts or when spawned
void ADFItemBox::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADFItemBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ADFItemBox::OnCharacterOverlap);
}

void ADFItemBox::OnCharacterOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	WeaponItemClass = ADFWeapon::StaticClass();
	UE_LOG(LogTemp, Log, TEXT("Box boundary"));

	auto ABCharacter = Cast<ADFGhostKnight>(OtherActor);
	if(nullptr == WeaponItemClass)
	{
		UE_LOG(LogTemp, Log, TEXT("NULL"));
	}
	if (nullptr != ABCharacter && nullptr != WeaponItemClass)
	{
		if (ABCharacter->CanSetWeapon())
		{
			UE_LOG(LogTemp, Log, TEXT("Get a Weapon"));
			auto NewWeapon = GetWorld()->SpawnActor<ADFWeapon>(WeaponItemClass,
				FVector::ZeroVector, FRotator::ZeroRotator);
			ABCharacter->SetWeapon(NewWeapon);
			Effect->Activate(true);
			Box->SetHiddenInGame(true, true);
			SetActorEnableCollision(false);
			Effect->OnSystemFinished.AddDynamic(this,
				&ADFItemBox::OnEffectFinished);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Already has a Weapon"));
		}
	}
	
}
void ADFItemBox::OnEffectFinished(class UParticleSystemComponent* PSystem)
{
	Destroy();
}




