// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LT_HW2C_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void SetupInputComponent() override;

	void ActionQuitGame();
	void ActionDebugAccTime();
	void ActionDebugResetTime();
	void ActionDebugSlowTime();
};
