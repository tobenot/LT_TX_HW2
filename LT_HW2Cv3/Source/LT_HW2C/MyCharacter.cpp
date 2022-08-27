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
	��ҿɲٿ����ǽ�ɫ
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
	
	//Ĭ��ֵ
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

	//Ѫ����Ӧ������ɫ����
	UMaterialInstanceConstant* BaseMaterial = Cast<UMaterialInstanceConstant>(GetMesh()->GetMaterial(11));
	if (BaseMaterial)
	{
		ClothMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, GetWorld());
		GetMesh()->SetMaterial(11, ClothMaterial);
		GetMesh()->SetMaterial(13, ClothMaterial);
	}

	//���ֽ����
	if (IsUsingSword)
	{
		//�����Ž�
		Weapon_OnBack->SetVisibility(0, 0);
		Weapon_R->SetVisibility(1, 0);
		GetCharacterMovement()->MaxWalkSpeed = 220.0f;
	}
	else
	{
		//���Ž�
		Weapon_OnBack->SetVisibility(1, 0);
		Weapon_R->SetVisibility(0, 0);
		GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	}
	//��ȫ�ص�������һ��
	SaveLocation = GetActorLocation();
	//���ϼ�¼��ȫ�ص㣬�ڵ�����պ��Ѫ��λ
	FTimerHandle SaveLocationTimerHandle;
	GetWorldTimerManager().SetTimer(SaveLocationTimerHandle, this, &AMyCharacter::RecordSavePlace, 1.0f, true);

}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//��ͣ���жϽ�ɫ�Ƿ��Ϳգ��Ϳ�֮��λ
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
	//��CG��ɫ�������л�
	AniCharacter = Target;
}
void AMyCharacter::SwitchToCGPawn()
{
	//�л���CG��ɫ
	if (AniCharacter)
	{
		AniCharacter->SetActorTransform(GetActorTransform(), 0, nullptr, ETeleportType::TeleportPhysics);
		HideBody();
		AniCharacter->ShowBody();
	}
}
void AMyCharacter::SwitchToControlPawn()
{
	//�л����Լ����ɲٿؽ�ɫ
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
	//�����Լ�
	SetActorHiddenInGame(true);
	AddActorWorldOffset(FVector(0, 0, 5000.0f));
}
void AMyCharacter::ShowBody()
{
	//չʾ�Լ�
	SetActorHiddenInGame(false);
}

void AMyCharacter::RightMousePressAction()
{
	//�Ҽ�����
	if (!IsDead && !IsHitRecovering && IsUsingSword)
	{
		if (IsAttacking)
		{
			//����������ڽ��������������������
			if (0 == NowAttackCount)
			{
				ContinueAttack = 1;
			}
		}
		else
		{
			//���û�н��й����ͽ����Ҽ�����
			NowAttackCount = 0;
			IsAttacking = 1;
			AttackAction();
		}
	}
}
void AMyCharacter::LeftMousePressAction()
{
	//�������
	if (!IsDead && !IsHitRecovering && IsUsingSword)
	{
		if (IsAttacking)
		{
			//����������ڽ����Ҽ���������������
			if (1 == NowAttackCount)
			{
				ContinueAttack = 1;
			}
		}
		else
		{
			//���û�н��й����ͽ����������
			NowAttackCount = 1;
			IsAttacking = 1;
			AttackAction();
		}
	}
}

void AMyCharacter::AttackAction()
{
	//��������
	 
	//�ѽ�ɫ�Ƶ��������ת��
	FRotator TargetRot = GetFollowCamera()->GetComponentRotation();
	TargetRot.Roll = 0; 
	TargetRot.Pitch = 0;
	SetActorRotation(TargetRot);

	//���չ������Ͳ��Ŷ���
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
	//������ѡ���������ͽ�������
	if (ContinueAttack)
	{
		ContinueAttack = 0;
		AttackAction();
	}
}
void AMyCharacter::ResetCombo()
{
	//�����ҹ���״̬�����ܻ�����϶���ʱ����ʹ��
	NowAttackCount = 0;
	ContinueAttack = 0;
	IsAttacking = 0;

}
void AMyCharacter::ClearContinueAttack()
{
	//����������֮ǰ����Ҽ�ʹ���˰���Ҳ������������Ϊ���������ڿ�ʼʱ�����ContinueAttack
	ContinueAttack = 0;
}
void AMyCharacter::MakeSwordDamage()
{
	//��������˺�
	
	//���߼��������յ�
	const FVector Start = GetActorLocation();
	const FVector End = GetActorForwardVector() * 128.0f + Start;
	const float TraceRadius = (NowAttackCount==0)?200.0f:256.0f;
	const bool IgnoreSelf = 1;
	//��Ҫ����actors
	TArray<AActor*> ActorsToIgnore;
	//��actor�����
	ActorsToIgnore.Add(this);
	//���Ľ��
	TArray<FHitResult> HitArray;
	//Ҫ��������
	const TArray<TEnumAsByte<EObjectTypeQuery>> DesireObjects = {
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic),
UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)
	};
	//���Ľ��
	const bool Hit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), Start, End, TraceRadius,
		DesireObjects, false, ActorsToIgnore, EDrawDebugTrace::None, HitArray, IgnoreSelf,
		FLinearColor::Red, FLinearColor::Green, 5.0f);

	if (Hit)
	{
		for (const FHitResult HitResult : HitArray)
		{
			//���֮���ѭ������
			auto Enemy = Cast<AMyEnemy>(HitResult.Actor);
			if (Enemy)
			{
				//�Ե�������˺�
				Enemy->SwordSlice();

			}
		}
	}

}

void AMyCharacter::DrawTheSword()
{
	//������ջؽ�����Ҳ������������������
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

						//���Ž�
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
						//�����Ž�
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
	//��Ϊ�Ϳն���λ����ȫλ��
	SetActorLocation(SaveLocation, 0, nullptr, ETeleportType::TeleportPhysics);
	TakeDamageFromMovingEnemy((-1.0f) * GetActorForwardVector(), 25.0f);
}

void AMyCharacter::RecordSavePlace()
{
	//��¼���Ϳյİ�ȫλ��
	if (!(GetCharacterMovement()->IsFalling()))
	{
		SaveLocation = GetActorLocation();
	}
}

void AMyCharacter::TakeDamageFromMovingEnemy(FVector Impulse, float Damage)
{
	//�ܵ����Ե��˵��˺�
	
	//�õ���ж��˺�����
	if (UKismetMathLibrary::Dot_VectorVector(GetActorForwardVector(),UKismetMathLibrary::GetDirectionUnitVector(FVector(0), Impulse)) < 0 )
	{
		if (UKismetMathLibrary::Dot_VectorVector(GetActorRightVector(),UKismetMathLibrary::GetDirectionUnitVector(FVector(0), Impulse)) > 0 )
		{
			//��ǰ���ܻ�
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
			//��ǰ���ܻ�
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
		//���ܻ�
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
	//��������
	Health = Health - Damage;
	if (Health < 0)
	{
		Health = 0;
	}
	else if (Health > HealthMax)
	{
		Health = HealthMax;
	}
	//��������ֵ����������ɫ
	UpdateClothesByHealth();
	//����ֵ���������������¿�ʼ��Ϸ
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
					//���¿�ʼ��Ϸ
					UGameplayStatics::OpenLevel(this, FName(UGameplayStatics::GetCurrentLevelName(this)));
				}, delayTime, false);
			
		}
	}
}
void AMyCharacter::Heal(float HealAmount)
{
	//�ָ�����ֵ
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
	//�ܻ�ʱ�䣬���ʱ�䲻�ܹ���
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
	//�ܻ�ʱ����������Թ���
	IsHitRecovering = 0;
}
void AMyCharacter::UpdateClothesByHealth()
{
	//��������ֵ����������ɫ
	if (ClothMaterial)
	{
		ClothMaterial->SetScalarParameterValue("Blend Color Power 01", (HealthMax - Health) / (HealthMax) * -0.5f);
	}

}

void AMyCharacter::PickUpAni()
{
	//��ƿ�Ӻȵĵ��Ӷ���Ȩ��
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
	//��ƿ�Ӻȵĵ��Ӷ�������
	PickUpAlpha = 0.0f;
}