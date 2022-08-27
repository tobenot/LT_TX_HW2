// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyAniCharacter.h"
#include "GameFramework/PawnMovementComponent.h"
#include "MyAnimInstance_Ghost.generated.h"

/**
 * 
 */
UCLASS()
class LT_HW2C_API UMyAnimInstance_Ghost : public UAnimInstance
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
		AMyAniCharacter* AniCharacter;
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

	UFUNCTION(BlueprintCallable)
		void CPPUpdateAnimation(float DeltaTimeX);

};
