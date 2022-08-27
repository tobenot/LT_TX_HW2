// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAniCharacter_Hero.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
/*
	���ǵ�CG��ɫ
*/
AMyAniCharacter_Hero::AMyAniCharacter_Hero()
{
	//����λ�õĽ�
	Weapon_R = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon_R"));
	Weapon_R->SetupAttachment(GetMesh(), FName(TEXT("WEAPON_R")));

	Weapon_OnBack = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon_OnBack"));
	Weapon_OnBack->SetupAttachment(GetMesh(), FName(TEXT("BOWCASE")));
	Weapon_OnBack->SetRelativeLocationAndRotation(FVector(-32.0, 5.0, 23.0), FRotator(15.0, -98, 124.0));


	ShortHair = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShortHair"));
	ShortHair->SetupAttachment(GetMesh(), FName(TEXT("HAIR")));

	//Ϊ�˺Ϳɲٿؽ�ɫ���ɣ����ӽ�������
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

}
// Called when the game starts or when spawned
void AMyAniCharacter_Hero::BeginPlay()
{
	Super::BeginPlay();
	//��ʼ���ֽ����
	if (IsUsingSword)
	{
		//�����Ž�
		Weapon_OnBack->SetVisibility(0, 0);
		Weapon_R->SetVisibility(1, 0);
	}
	else
	{
		//���Ž�
		Weapon_OnBack->SetVisibility(1, 0);
		Weapon_R->SetVisibility(0, 0);
	}
}


void AMyAniCharacter_Hero::DrawTheSword()
{
	//��/�ս�
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
						
						IsDrawingSword = 0;

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
						IsDrawingSword = 0;
						IsUsingSword = 1;
					}, delayTime, false);

			}



		}
	}
}


void AMyAniCharacter_Hero::FastDrawTheSword()
{
	if (!IsDrawingSword)
	{
		IsDrawingSword = 1;
		if (IsUsingSword)
		{
			IsUsingSword = 0;
			//���Ž�
			Weapon_OnBack->SetVisibility(1, 0);
			Weapon_R->SetVisibility(0, 0);
			IsDrawingSword = 0;

		}
		else
		{
			//�����Ž�
			Weapon_OnBack->SetVisibility(0, 0);
			Weapon_R->SetVisibility(1, 0);
			
			IsDrawingSword = 0;
			IsUsingSword = 1;

		}
	}
}