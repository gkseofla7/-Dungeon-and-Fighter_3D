// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Styling/SlateBrush.h"
#include "DFInventoryWidget.generated.h"


/**
 * 
 */
UCLASS()
class DUNGEONAFIGHTER_3D_API UDFInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FSlateBrush GetInventoryImage() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool InventoryCheck() const;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* PickupImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InventorySlot;
};
