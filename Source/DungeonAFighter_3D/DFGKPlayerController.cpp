// Fill out your copyright notice in the Description page of Project Settings.


#include "DFGKPlayerController.h"

void ADFGKPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}