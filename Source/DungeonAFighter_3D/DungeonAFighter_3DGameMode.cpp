// Copyright Epic Games, Inc. All Rights Reserved.

#include "DungeonAFighter_3DGameMode.h"
#include "DungeonAFighter_3DCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADungeonAFighter_3DGameMode::ADungeonAFighter_3DGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/SideScrollerCPP/Blueprints/SideScrollerCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
