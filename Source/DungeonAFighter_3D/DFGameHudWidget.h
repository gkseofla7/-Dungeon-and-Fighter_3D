// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryStruct.h"
#include "DFGameHudWidget.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONAFIGHTER_3D_API UDFGameHudWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ActivateInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESlateVisibility InventoryVisible = ESlateVisibility::Hidden;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESlateVisibility ActionMenuVisible = ESlateVisibility::Hidden;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ActionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InventorySlotClicked = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInventoryStruct> Inventory;


};
