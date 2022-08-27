// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyAniCharacter.h"
#include "MyAniCharacter_Hero.generated.h"

/**
 * 
 */
UCLASS()
class LT_HW2C_API AMyAniCharacter_Hero : public AMyAniCharacter
{
	GENERATED_BODY()

	/** Weapons */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SMesh, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Weapon_R;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SMesh, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* Weapon_OnBack;

	/** Hair */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SMesh, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* ShortHair;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;


public:

	AMyAniCharacter_Hero();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Atk)
		bool IsUsingSword;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		bool IsDrawingSword;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Montage)
		class UAnimMontage* EquipMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Montage)
		class UAnimMontage* UnEquipMontage;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
		void DrawTheSword();
	UFUNCTION(BlueprintCallable)
		void FastDrawTheSword();
};
