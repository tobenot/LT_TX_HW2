// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "Kismet/GameplayStatics.h"

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("QuitGame", IE_Pressed, this, &AMyPlayerController::ActionQuitGame);
	InputComponent->BindAction("AccTime", IE_Pressed, this, &AMyPlayerController::ActionDebugAccTime);
	InputComponent->BindAction("ResetTime", IE_Pressed, this, &AMyPlayerController::ActionDebugResetTime);
	InputComponent->BindAction("SlowTime", IE_Pressed, this, &AMyPlayerController::ActionDebugSlowTime);
}

void AMyPlayerController::ActionQuitGame()
{
	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
}
void AMyPlayerController::ActionDebugAccTime()
{
	//时间流速加快
	if (UGameplayStatics::GetGlobalTimeDilation(this) < 6.0f)
	{
		UGameplayStatics::SetGlobalTimeDilation(this, UGameplayStatics::GetGlobalTimeDilation(this)+0.5f);
	}
	
}
void AMyPlayerController::ActionDebugResetTime()
{
	//时间流速恢复正常
	UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);
}
void AMyPlayerController::ActionDebugSlowTime()
{
	//时间流速慢速
	UGameplayStatics::SetGlobalTimeDilation(this, 0.25f);
}