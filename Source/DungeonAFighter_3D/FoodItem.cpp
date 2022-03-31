// Fill out your copyright notice in the Description page of Project Settings.


#include "FoodItem.h"
#include "DFGhostKnight.h"
void UFoodItem::Use(class ADFGhostKnight* Character)
{
	if (Character)
	{
		Character->DrinkHp(HeathToHeal);
	}
}