// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyCharacter.h"
#include "MyAniCharacter.h"
#include "GameFramework/PawnMovementComponent.h"
#include "MyAnimInstance_Hero.generated.h"

/**
 * 
 */
UCLASS()
class LT_HW2C_API UMyAnimInstance_Hero : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		bool IsInAir;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		float Speed;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		float Direction;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		float Pitch;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		float Yaw;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		float PickUpAlpha;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		AMyCharacter* PlayerPawn;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		AMyAniCharacter* AniCharacter;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		FVector LeftHandInterEffectorLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		float LeftHandInterAlpha;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		FVector LeftHandIKEffectorLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		float LeftHandIKAlpha;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		FVector RightHandIKEffectorLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		float RightHandIKAlpha;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		float IKSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		bool IsAniCharacter;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		bool IsUsingSword;
	
	UFUNCTION(BlueprintCallable)
		void CPPUpdateAnimation(float DeltaTimeX);

	

};
