// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyAniCharacter.h"
#include "MyAniCharacter_Ghost.generated.h"

/**
 * 
 */
UCLASS()
class LT_HW2C_API AMyAniCharacter_Ghost : public AMyAniCharacter
{
	GENERATED_BODY()
	/** Weapons */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SMesh, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* SM_GreatSword_HandR;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SMesh, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* SM_greatswordsheath;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SMesh, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* SM_greatswordsheath_HandL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SMesh, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* SM_GreatSword_HandL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SMesh, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* SM_GreatSword;

public:

	AMyAniCharacter_Ghost();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Atk)
		bool IsUsingSword;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		bool IsDrawingSword;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		bool LeftHandSword;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Montage)
		class UAnimMontage* EquipMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Montage)
		class UAnimMontage* UnEquipMontage;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
		void DrawTheSword(bool UneqiupToLeftHand);
};
