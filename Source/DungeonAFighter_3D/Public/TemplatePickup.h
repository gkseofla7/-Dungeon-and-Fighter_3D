// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryStruct.h"
#include "TemplatePickup.generated.h"

UCLASS()
class DUNGEONAFIGHTER_3D_API ATemplatePickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATemplatePickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsInRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInventoryStruct ItemInfo;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
