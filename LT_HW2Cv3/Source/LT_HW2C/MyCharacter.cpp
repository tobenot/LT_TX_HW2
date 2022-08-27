// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MyEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

/*
	玩家可操控主角角色
*/
AMyCharacter::AMyCharacter()
{

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Weapon_R = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon_R"));
	Weapon_R->SetupAttachment(GetMesh(), FName(TEXT("WEAPON_R")));

	Weapon_OnBack = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon_OnBack"));
	Weapon_OnBack->SetupAttachment(GetMesh(), FName(TEXT("BOWCASE")));
	Weapon_OnBack->SetRelativeLocationAndRotation(FVector(-32.0, 5.0, 23.0), FRotator(15.0,-98,124.0));


	ShortHair = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShortHair"));
	ShortHair->SetupAttachment(GetMesh(), FName(TEXT("HAIR")));
	GetCharacterMovement()->MaxWalkSpeed = 1.0f;
	
	//默认值
	IsUsingSword = 1;
	Health = 100.0f;
	HealthMax = 100.0f;
	LeftHandIK = 1;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	//血量对应披风颜色材质
	UMaterialInstanceConstant* BaseMaterial = Cast<UMaterialInstanceConstant>(GetMesh()->GetMaterial(11));
	if (BaseMaterial)
	{
		ClothMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, GetWorld());
		GetMesh()->SetMaterial(11, ClothMaterial);
		GetMesh()->SetMaterial(13, ClothMaterial);
	}

	//检测持剑情况
	if (IsUsingSword)
	{
		//手拿着剑
		Weapon_OnBack->SetVisibility(0, 0);
		Weapon_R->SetVisibility(1, 0);
		GetCharacterMovement()->MaxWalkSpeed = 220.0f;
	}
	else
	{
		//背着剑
		Weapon_OnBack->SetVisibility(1, 0);
		Weapon_R->SetVisibility(0, 0);
		GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	}
	//安全地点先设置一个
	SaveLocation = GetActorLocation();
	//不断记录安全地点，在掉下虚空后扣血复位
	FTimerHandle SaveLocationTimerHandle;
	GetWorldTimerManager().SetTimer(SaveLocationTimerHandle, this, &AMyCharacter::RecordSavePlace, 1.0f, true);

}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//不停地判断角色是否滞空，滞空之后复位
	if (GetCharacterMovement()->IsFalling())
	{
		if (!(GetWorld()->GetTimerManager().IsTimerActive(FailingTimer)))
		{
			GetWorldTimerManager().SetTimer(FailingTimer, this, &AMyCharacter::FallingDead, 0.5f, false);
		}

	}
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("LeftAtk", IE_Pressed, this, &AMyCharacter::LeftMousePressAction);
	PlayerInputComponent->BindAction("RightAtk", IE_Pressed, this, &AMyCharacter::RightMousePressAction);
}

void AMyCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMyCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AMyCharacter::BindAniCharacter(AMyAniCharacter* Target)
{
	//绑定CG角色，用于切换
	AniCharacter = Target;
}
void AMyCharacter::SwitchToCGPawn()
{
	//切换到CG角色
	if (AniCharacter)
	{
		AniCharacter->SetActorTransform(GetActorTransform(), 0, nullptr, ETeleportType::TeleportPhysics);
		HideBody();
		AniCharacter->ShowBody();
	}
}
void AMyCharacter::SwitchToControlPawn()
{
	//切换到自己，可操控角色
	if (AniCharacter)
	{
		SetActorTransform(AniCharacter->GetActorTransform(), 0, nullptr, ETeleportType::TeleportPhysics);
		AniCharacter->HideBody();
		if (PlayerController)
		{
			PlayerController->SetViewTarget(this);
			EnableInput(PlayerController);
		}
		ShowBody();
	}
}

void AMyCharacter::HideBody()
{
	//隐藏自己
	SetActorHiddenInGame(true);
	AddActorWorldOffset(FVector(0, 0, 5000.0f));
}
void AMyCharacter::ShowBody()
{
	//展示自己
	SetActorHiddenInGame(false);
}

void AMyCharacter::RightMousePressAction()
{
	//右键攻击
	if (!IsDead && !IsHitRecovering && IsUsingSword)
	{
		if (IsAttacking)
		{
			//如果现在正在进行左键攻击，可以连击
			if (0 == NowAttackCount)
			{
				ContinueAttack = 1;
			}
		}
		else
		{
			//如果没有进行攻击就进行右键攻击
			NowAttackCount = 0;
			IsAttacking = 1;
			AttackAction();
		}
	}
}
void AMyCharacter::LeftMousePressAction()
{
	//左键攻击
	if (!IsDead && !IsHitRecovering && IsUsingSword)
	{
		if (IsAttacking)
		{
			//如果现在正在进行右键攻击，可以连击
			if (1 == NowAttackCount)
			{
				ContinueAttack = 1;
			}
		}
		else
		{
			//如果没有进行攻击就进行左键攻击
			NowAttackCount = 1;
			IsAttacking = 1;
			AttackAction();
		}
	}
}

void AMyCharacter::AttackAction()
{
	//攻击动作
	 
	//把角色移到朝向的旋转上
	FRotator TargetRot = GetFollowCamera()->GetComponentRotation();
	TargetRot.Roll = 0; 
	TargetRot.Pitch = 0;
	SetActorRotation(TargetRot);

	//按照攻击类型播放动作
	switch (NowAttackCount)
	{
	case 0:
		NowAttackCount = 1;
		if (LeftAtkMontage)
		{
			PlayAnimMontage(LeftAtkMontage);
		}
		break;
	case 1:
		NowAttackCount = 0;
		if (RightAtkMontage)
		{
			PlayAnimMontage(RightAtkMontage);
		}
		break;
	default:
		break;
	}
}

void AMyCharacter::ContinueAttackEvent()
{
	//如果玩家选择连击，就进行连击
	if (ContinueAttack)
	{
		ContinueAttack = 0;
		AttackAction();
	}
}
void AMyCharacter::ResetCombo()
{
	//清空玩家攻击状态，在受击、打断动作时可以使用
	NowAttackCount = 0;
	ContinueAttack = 0;
	IsAttacking = 0;

}
void AMyCharacter::ClearContinueAttack()
{
	//在连击窗口之前，玩家即使按了按键也不能连击，因为在连击窗口开始时清空了ContinueAttack
	ContinueAttack = 0;
}
void AMyCharacter::MakeSwordDamage()
{
	//攻击造成伤害
	
	//射线检测的起点和终点
	const FVector Start = GetActorLocation();
	const FVector End = GetActorForwardVector() * 128.0f + Start;
	const float TraceRadius = (NowAttackCount==0)?200.0f:256.0f;
	const bool IgnoreSelf = 1;
	//不要检测的actors
	TArray<AActor*> ActorsToIgnore;
	//本actor不检测
	ActorsToIgnore.Add(this);
	//检查的结果
	TArray<FHitResult> HitArray;
	//要检测的类型
	const TArray<TEnumAsByte<EObjectTypeQuery>> DesireObjects = {
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic),
UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)
	};
	//检测的结果
	const bool Hit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), Start, End, TraceRadius,
		DesireObjects, false, ActorsToIgnore, EDrawDebugTrace::None, HitArray, IgnoreSelf,
		FLinearColor::Red, FLinearColor::Green, 5.0f);

	if (Hit)
	{
		for (const FHitResult HitResult : HitArray)
		{
			//检测之后的循环处理
			auto Enemy = Cast<AMyEnemy>(HitResult.Actor);
			if (Enemy)
			{
				//对敌人造成伤害
				Enemy->SwordSlice();

			}
		}
	}

}

void AMyCharacter::DrawTheSword()
{
	//抽出、收回剑，玩家不能主动进行这个动作
	if (!IsDrawingSword)
	{
		IsDrawingSword = 1;
		if (IsUsingSword)
		{
			IsUsingSword = 0;
			if (UnEquipMontage)
			{
				PlayAnimMontage(UnEquipMontage, 2.0f);

				FTimerHandle TimerHandle; float delayTime =
					1.70f * 0.5f;//Delay
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
					{
						//Delay Execute

						//背着剑
						Weapon_OnBack->SetVisibility(1, 0);
						Weapon_R->SetVisibility(0, 0);
						GetCharacterMovement()->MaxWalkSpeed = 300.0f;

						IsDrawingSword = 0;
						ResetCombo();

					}, delayTime, false);

			}
		}
		else
		{
			if (EquipMontage)
			{
				PlayAnimMontage(EquipMontage, 2.0f);

				FTimerHandle TimerHandle; float delayTime =
					0.80f * 0.5f;//Delay
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
					{
						//Delay Execute
						// 
						//手拿着剑
						Weapon_OnBack->SetVisibility(0, 0);
						Weapon_R->SetVisibility(1, 0);
						GetCharacterMovement()->MaxWalkSpeed = 220.0f;

						IsDrawingSword = 0;
						ResetCombo();
						IsUsingSword = 1;
					}, delayTime, false);

			}



		}
	}
}

void AMyCharacter::FallingDead()
{
	//因为滞空而复位到安全位置
	SetActorLocation(SaveLocation, 0, nullptr, ETeleportType::TeleportPhysics);
	TakeDamageFromMovingEnemy((-1.0f) * GetActorForwardVector(), 25.0f);
}

void AMyCharacter::RecordSavePlace()
{
	//记录不滞空的安全位置
	if (!(GetCharacterMovement()->IsFalling()))
	{
		SaveLocation = GetActorLocation();
	}
}

void AMyCharacter::TakeDamageFromMovingEnemy(FVector Impulse, float Damage)
{
	//受到来自敌人的伤害
	
	//用点乘判断伤害方向
	if (UKismetMathLibrary::Dot_VectorVector(GetActorForwardVector(),UKismetMathLibrary::GetDirectionUnitVector(FVector(0), Impulse)) < 0 )
	{
		if (UKismetMathLibrary::Dot_VectorVector(GetActorRightVector(),UKismetMathLibrary::GetDirectionUnitVector(FVector(0), Impulse)) > 0 )
		{
			//左前方受击
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("LF Hurt"));
			if (!IsHitRecovering)
			{
				RecoverFromHit(0.5f);
				if (LS_Dame_02_Montage)
				{
					PlayAnimMontage(LS_Dame_02_Montage);
				}
			}
		}
		else
		{
			//右前方受击
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("RF Hurt"));
			if (!IsHitRecovering)
			{
				RecoverFromHit(0.5f);
				if (LS_Dame_01_Montage)
				{
					PlayAnimMontage(LS_Dame_01_Montage);
				}
				
			}
		}
	}
	else
	{
		//后方受击
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Back Hurt"));
		if (!IsHitRecovering)
		{
			RecoverFromHit(1.5f);
			if (LS_RollFwd_Montage)
			{
				PlayAnimMontage(LS_RollFwd_Montage);
			}
			
		}
	}
	//减少生命
	Health = Health - Damage;
	if (Health < 0)
	{
		Health = 0;
	}
	else if (Health > HealthMax)
	{
		Health = HealthMax;
	}
	//根据生命值更新披风颜色
	UpdateClothesByHealth();
	//生命值归零则死亡，重新开始游戏
	if (Health <= 0)
	{
		if (!IsDead)
		{
			IsDead = 1;
			ResetCombo();
			if (LS_Death_Montage)
			{
				PlayAnimMontage(LS_Death_Montage);
			}
			FTimerHandle TimerHandle; float delayTime =
				3.0f;//Delay
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
				{
					//Delay Execute
					//重新开始游戏
					UGameplayStatics::OpenLevel(this, FName(UGameplayStatics::GetCurrentLevelName(this)));
				}, delayTime, false);
			
		}
	}
}
void AMyCharacter::Heal(float HealAmount)
{
	//恢复生命值
	Health = Health + HealAmount;
	if (Health < 0)
	{
		Health = 0;
	}
	else if (Health > HealthMax)
	{
		Health = HealthMax;
	}
	UpdateClothesByHealth();
}
void AMyCharacter::RecoverFromHit(float Duration)
{
	//受击时间，这个时间不能攻击
	IsHitRecovering = 1;
	ResetCombo();

	if (!(GetWorld()->GetTimerManager().IsTimerActive(RecoverTimer)))
	{
		GetWorldTimerManager().SetTimer(RecoverTimer, this, &AMyCharacter::RecoverFromHitEnd, Duration, false);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(RecoverTimer);
		GetWorldTimerManager().SetTimer(RecoverTimer, this, &AMyCharacter::RecoverFromHitEnd, Duration, false);
	}
}
void AMyCharacter::RecoverFromHitEnd()
{
	//受击时间结束，可以攻击
	IsHitRecovering = 0;
}
void AMyCharacter::UpdateClothesByHealth()
{
	//根据生命值更新披风颜色
	if (ClothMaterial)
	{
		ClothMaterial->SetScalarParameterValue("Blend Color Power 01", (HealthMax - Health) / (HealthMax) * -0.5f);
	}

}

void AMyCharacter::PickUpAni()
{
	//捡瓶子喝的叠加动画权重
	PickUpAlpha = 1.0f;
	if (!(GetWorld()->GetTimerManager().IsTimerActive(PickUpAniTimer)))
	{
		GetWorldTimerManager().SetTimer(PickUpAniTimer, this, &AMyCharacter::PickUpAniEnd, 0.67f, false);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(PickUpAniTimer);
		GetWorldTimerManager().SetTimer(PickUpAniTimer, this, &AMyCharacter::PickUpAniEnd, 0.67f, false);
	}
}
void AMyCharacter::PickUpAniEnd()
{
	//捡瓶子喝的叠加动画结束
	PickUpAlpha = 0.0f;
}