// Copyright Epic Games, Inc. All Rights Reserved.

#include "LogicGatesGameMode.h"
#include "LogicGatesCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALogicGatesGameMode::ALogicGatesGameMode()
{
	// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/LogicGates/Blueprints/BP_LogicGatesCharacter"));
	
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
