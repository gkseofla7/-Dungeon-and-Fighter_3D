// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DFGKPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONAFIGHTER_3D_API ADFGKPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
};
