// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyAniCharacter.generated.h"

UCLASS()
class LT_HW2C_API AMyAniCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyAniCharacter();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		FRotator TargetRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		bool IsTurning;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		FVector DisplacementVector;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		float DisplacementSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Montage)
		bool EnableBodyIK;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		bool IsDisplacementing;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void TurnToRotation(FRotator tTargetRotation);
	void StopTurn();

	void HideBody();
	void ShowBody();

	void Displacement(FVector Vector, float Duration);


};
