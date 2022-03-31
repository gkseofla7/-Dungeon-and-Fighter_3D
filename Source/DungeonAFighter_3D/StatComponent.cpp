// Fill out your copyright notice in the Description page of Project Settings.


#include "StatComponent.h"
#include "DFGameInstance.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UStatComponent::UStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	bWantsInitializeComponent = true;

	Level = 1;
}


// Called when the game starts
void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetLevel(Level);
}

void UStatComponent::SetLevel(int32 NewLevel)
{
	auto GameInstance = Cast<UDFGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GameInstance)
	{
		auto StatData = GameInstance->GetStatData(NewLevel);
		if (StatData)
		{
			Level = StatData->Level;
			MaxHp = StatData->MaxHp;
			SetHp(MaxHp);
			Attack = StatData->Attack;
		}
	}

}

void UStatComponent::SetHp(int32 NewHp)
{
	Hp = NewHp;
	if (Hp < 0)
		Hp = 0;
	if (Hp>MaxHp)
	{
		Hp = MaxHp;
	}

	OnHpChanged.Broadcast();
}

void UStatComponent::OnAttacked(float DamageAmount)
{
	int32 NewHp = Hp - DamageAmount;
	SetHp(NewHp);


	//UE_LOG(LogTemp, Warning, TEXT("OnAttacked %d"), Hp);
}

void UStatComponent::DrinkHp(float DrinkAmount)
{
	int32 NewHp = Hp + DrinkAmount;
	SetHp(NewHp);
}




