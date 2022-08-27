// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAniCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

/*
	CG用角色,用于在CG中播放动作
*/

// Sets default values
AMyAniCharacter::AMyAniCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyAniCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyAniCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsTurning)
	{
		//转向处理
		FRotator tp = (UKismetMathLibrary::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 3.0f));
		tp.Pitch = 0;
		tp.Roll = 0;
		SetActorRotation(tp);
	}
	if (IsDisplacementing)
	{
		//位移处理
		AddActorWorldOffset(DisplacementVector * DisplacementSpeed * DeltaTime);
	}
	

}

// Called to bind functionality to input
void AMyAniCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMyAniCharacter::TurnToRotation(FRotator tTargetRotation)
{
	//旋转到指定方向
	TargetRotation = tTargetRotation;
	IsTurning = 1;
	FTimerHandle TimerHandle; float delayTime =
		3.0f;//Delay
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			//Delay Execute
			IsTurning = 0;
		}, delayTime, false);
}
void AMyAniCharacter::StopTurn()
{
	//停止旋转
	IsTurning = 0;
}

void AMyAniCharacter::HideBody()
{
	//隐藏自己的角色
	SetActorHiddenInGame(1);
	AddActorWorldOffset(FVector(0, 0, 5000), 0, nullptr, ETeleportType::TeleportPhysics);
	GetCharacterMovement()->GravityScale = 0.0f;
	GetCharacterMovement()->Velocity = FVector(0);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}
void AMyAniCharacter::ShowBody()
{
	//展现自己的角色
	SetActorHiddenInGame(0);
	GetCharacterMovement()->GravityScale = 1.0f;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);

}

void AMyAniCharacter::Displacement(FVector Vector, float Duration)
{
	//位移
	if (Duration <= 0.05)
	{
		AddActorWorldOffset(Vector);
	}
	else
	{
		DisplacementSpeed = (1 / Duration);
		DisplacementVector = Vector;
		IsDisplacementing = 1;
		//未经缜密设计，不频繁使用就没问题
		FTimerHandle TimerHandle; float delayTime =
			Duration;//Delay
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
			{
				//Delay Execute
				IsDisplacementing = 0;
			}, delayTime, false);
	}
}