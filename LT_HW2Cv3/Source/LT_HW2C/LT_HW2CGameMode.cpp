// Copyright Epic Games, Inc. All Rights Reserved.

#include "LT_HW2CGameMode.h"
#include "LT_HW2CCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALT_HW2CGameMode::ALT_HW2CGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
