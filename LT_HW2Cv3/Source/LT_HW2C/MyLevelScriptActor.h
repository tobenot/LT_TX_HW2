// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "MyAniCharacter.h"
#include "MyAniCharacter_Hero.h"
#include "MyAniCharacter_Ghost.h"
#include "Engine/TriggerBox.h"
#include "Components/ShapeComponent.h"
#include "Engine/StaticMeshActor.h"
#include "MyCharacter.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyLevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class LT_HW2C_API AMyLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	AMyLevelScriptActor();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = CG)
		TArray<UAnimMontage*> AllCGMontages;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = CG)
		TArray<AActor*> TargetPoints;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = CG)
		AMyAniCharacter_Ghost* Ghost;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = CG)
		AMyAniCharacter_Hero* Hero;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Ref)
		AAIController* GhostC;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Ref)
		AAIController* HeroC;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = CG)
		TArray<ATriggerBox*> TriggerBoxs;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = CG)
		TArray<AActor*> TargetPointss;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = CG)
		AStaticMeshActor* Door;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = CG)
		AStaticMeshActor* Chair1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = CG)
		AStaticMeshActor* Chair2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = CG)
		TArray<AActor*> Camerass;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Ref)
		AMyCharacter* PlayerPawn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Ref)
		APlayerController* PC;

	UFUNCTION()
		void DoorOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
		void TriggerBox1_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void TriggerBox2_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void TriggerBox3_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void TriggerBox4_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	UFUNCTION()
		void ChangeCamera(int id);
	UFUNCTION()
		void ChangeCameraTime(int id,float time);
	UFUNCTION()
		void GhostPlayMontage(int id);
	UFUNCTION()
		void HeroPlayMontage(int id);
	UFUNCTION()
		void GhostMoveTo(int id, float dis);
	UFUNCTION()
		void HeroMoveTo(int id, float dis);
	UFUNCTION()
		void GhostMoveToD(int id);
	UFUNCTION()
		void HeroMoveToD(int id);
	UFUNCTION()
		void WalkClose(float dis);
	UFUNCTION()
		void HeroSetTo(int id);
	UFUNCTION()
		void GhostSetTo(int id);
};
