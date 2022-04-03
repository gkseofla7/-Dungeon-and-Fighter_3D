// Fill out your copyright notice in the Description page of Project Settings.

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "DFInventoryWidget.h"
#include "Kismet/KismetSystemLibrary.h"

FSlateBrush UDFInventoryWidget::GetInventoryImage() const
{
	
	return UWidgetBlueprintLibrary::MakeBrushFromTexture(PickupImage);
}

bool UDFInventoryWidget::InventoryCheck() const
{
	
	return UKismetSystemLibrary::IsValid(PickupImage);
}