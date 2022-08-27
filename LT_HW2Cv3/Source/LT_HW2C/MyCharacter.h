// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyAniCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "MyPlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MyCharacter.generated.h"


UCLASS()
class LT_HW2C_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	/** Weapons */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SMesh, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* Weapon_R;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SMesh, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* Weapon_OnBack;

	/** Hair */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SMesh, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* ShortHair;

	

public:
	// Sets default values for this character's properties
	AMyCharacter();



	//±£¥ÊAniCharacter“˝”√
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		class AMyAniCharacter* AniCharacter;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Atk)
		bool IsAttacking;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Atk)
		bool ContinueAttack;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Atk)
		int NowAttackCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Atk)
		bool IsUsingSword;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		bool IsDrawingSword;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Battle)
		bool IsDead;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Battle)
		bool IsHitRecovering;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Battle)
		float Health;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Battle)
		float HealthMax;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Battle)
		UMaterialInstanceDynamic* ClothMaterial;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Battle)
		FVector SaveLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Battle)
		FTimerHandle RecoverTimer;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		FTimerHandle FailingTimer;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		FTimerHandle PickUpAniTimer;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		bool LeftHandIK;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		float PickUpAlpha;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation)
		class AMyPlayerController* PlayerController;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Montage)
		class UAnimMontage* LeftAtkMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Montage)
		class UAnimMontage* RightAtkMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Montage)
		class UAnimMontage* EquipMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Montage)
		class UAnimMontage* UnEquipMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Montage)
		class UAnimMontage* LS_Dame_01_Montage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Montage)
		class UAnimMontage* LS_Dame_02_Montage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Montage)
		class UAnimMontage* LS_RollFwd_Montage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Montage)
		class UAnimMontage* LS_Death_Montage;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const
	{
		return CameraBoom;
	}
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const
	{
		return FollowCamera;
	}


	void BindAniCharacter(AMyAniCharacter* Target);
	void SwitchToCGPawn();
	void SwitchToControlPawn();

	void HideBody();
	void ShowBody();

	void RightMousePressAction();
	void LeftMousePressAction();

	void AttackAction();
	void ContinueAttackEvent();
	void ResetCombo();
	void ClearContinueAttack();
	void MakeSwordDamage();

	UFUNCTION(BlueprintCallable)
	void DrawTheSword();

	void FallingDead();
	void RecordSavePlace();

	void TakeDamageFromMovingEnemy(FVector Impulse, float Damage);
	void Heal(float HealAmount);
	void RecoverFromHitEnd();
	void RecoverFromHit(float Duration);
	void UpdateClothesByHealth();
	void PickUpAniEnd();
	void PickUpAni();

};
