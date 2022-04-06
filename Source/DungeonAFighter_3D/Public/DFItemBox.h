// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Engine/Classes/Particles/ParticleSystemComponent.h"
#include "DFItemBox.generated.h"


UCLASS()
class DUNGEONAFIGHTER_3D_API ADFItemBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADFItemBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;


public:
	UPROPERTY(VisibleAnywhere, Category = Box)
		UBoxComponent* Trigger;

	UPROPERTY(VisibleAnywhere, Category = Box)
		UStaticMeshComponent* Box;

	UPROPERTY(VisibleAnywhere, Category = Effect)
		UParticleSystemComponent* Effect;
	UPROPERTY(EditInstanceOnly, Category = Box)
		TSubclassOf<class ADFWeapon> WeaponItemClass;
	//UPROPERTY(EditInstanceOnly, Category = Box)
	//	UClass* WeaponItemClass;
private:
	UFUNCTION()
		void OnCharacterOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp, 
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	UFUNCTION()
		void OnEffectFinished(class UParticleSystemComponent* PSystem);

};
