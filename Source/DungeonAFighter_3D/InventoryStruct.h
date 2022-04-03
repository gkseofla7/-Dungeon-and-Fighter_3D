// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryStruct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct DUNGEONAFIGHTER_3D_API FInventoryStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Item;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ItemImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText PickupText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ActionText;
};
