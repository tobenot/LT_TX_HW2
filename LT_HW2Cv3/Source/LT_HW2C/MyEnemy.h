// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "MyEnemy.generated.h"

UCLASS()
class LT_HW2C_API AMyEnemy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyEnemy();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Enemy)
		float AttackDistance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Enemy)
		float UpLaunchSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Enemy)
		float FlySpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Enemy)
		float ImpulseMup;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Enemy)
		float Damage;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Battle)
		class APawn* Pawn;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Enemy)
		bool  IsDead;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Enemy)
		bool  HaveStartAttack;

	// 发射物移动组件。
	UPROPERTY(VisibleAnywhere, Category = Movement)
		UProjectileMovementComponent* ProjectileMovementComponent;

	// 静态网格体组件。
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* MeshComp;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void StartAttack();
	void DeadPhy();
	void SwordSlice();

	// 当发射物击中物体时会调用的函数。
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

};
