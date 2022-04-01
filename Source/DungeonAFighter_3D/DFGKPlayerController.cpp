// Fill out your copyright notice in the Description page of Project Settings.


#include "DFGKPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "DFGameHudWidget.h"

ADFGKPlayerController::ADFGKPlayerController()
{
	static ConstructorHelpers::FClassFinder<UDFGameHudWidget> UI_HUD_C(TEXT("WidgetBlueprint'/Game/UI/GameHud.GameHud_C'"));
	if (UI_HUD_C.Succeeded())
	{
		HUDWidgetClass = UI_HUD_C.Class;
	}
}

void ADFGKPlayerController::BeginPlay()
{
	Super::BeginPlay();

	HUDWidget = CreateWidget<UDFGameHudWidget>(this, HUDWidgetClass);
	HUDWidget->AddToViewport(1);
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}