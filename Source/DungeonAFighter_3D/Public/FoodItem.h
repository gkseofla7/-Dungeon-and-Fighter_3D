// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "FoodItem.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONAFIGHTER_3D_API UFoodItem : public UItem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta = (Clamp = 0.0))
		float HeathToHeal;

protected:
	virtual void Use(class ADFGhostKnight* Character) override;
};
