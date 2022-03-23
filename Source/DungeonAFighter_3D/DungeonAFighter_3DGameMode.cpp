// Copyright Epic Games, Inc. All Rights Reserved.

#include "DungeonAFighter_3DGameMode.h"
#include "DungeonAFighter_3DCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "DFGhostKnight.h"

ADungeonAFighter_3DGameMode::ADungeonAFighter_3DGameMode()
{
	//// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/SideScrollerCPP/Blueprints/SideScrollerCharacter"));
	//if (PlayerPawnBPClass.Class != nullptr)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
	//}
	static ConstructorHelpers::FClassFinder<ACharacter> BP_Char(TEXT("Blueprint'/Game/Blueprints/BP_DFGhostKnight.BP_DFGhostKnight_C'"));
	if (BP_Char.Succeeded())
	{
		DefaultPawnClass = BP_Char.Class;
	}
	//DefaultPawnClass = ADFGhostKnight::StaticClass();
}
