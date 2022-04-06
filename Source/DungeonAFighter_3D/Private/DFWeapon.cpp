// Fill out your copyright notice in the Description page of Project Settings.


#include "DFWeapon.h"

// Sets default values
ADFWeapon::ADFWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	//USkeletalMeshComponent
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
	RootComponent = Weapon;
	//UStaticMesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WEAPON(TEXT("SkeletalMesh'/Game/InfinityBladeWeapons/Weapons/Blade/Axes/Blade_GreatBlade/SK_Blade_GreatBlade.SK_Blade_GreatBlade'"));

	if (SK_WEAPON.Succeeded())
	{
		Weapon->SetSkeletalMesh(SK_WEAPON.Object);
	}

	Weapon->SetCollisionProfileName(TEXT("NoCollision"));
}

// Called when the game starts or when spawned
void ADFWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}



