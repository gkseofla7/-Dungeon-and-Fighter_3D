// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONAFIGHTER_3D_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;

public:
	void SetLevel(int32 Level);
	void SetHp(int32 NewHp);
	void OnAttacked(float DamageAmount);
	void DrinkHp(float NewHp);
	void DrinkMp(float NewMp);


	int32 GetLevel() { return Level; }
	int32 GetHp() { return Hp; }
	int32 GetMp() { return Mp; }
	int32 GetMaxHp() { return MaxHp; }
	int32 GetMaxMp() { return MaxMp; }
	UFUNCTION(BlueprintCallable)
	float GetHpRatio() { return Hp / (float)MaxHp; }
	int32 GetAttack() { return Attack; }
	

private:
	UPROPERTY(EditAnywhere, Category = Stat, Meta=(AllowPrivateAccess = true))
	int32 Level;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 Hp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 Mp;
	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 MaxHp;
	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 MaxMp;
	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 Attack;



public:
	FOnHpChanged OnHpChanged;
		
};
