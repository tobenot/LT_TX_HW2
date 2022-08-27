// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAniCharacter_Ghost.h"

/*
	敌人的CG角色
*/
AMyAniCharacter_Ghost::AMyAniCharacter_Ghost()
{
	//各种位置的剑的模型
	SM_GreatSword_HandR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_GreatSword_HandR"));
	SM_GreatSword_HandR->SetupAttachment(GetMesh(), FName(TEXT("katana3")));
	SM_GreatSword_HandR->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));

	SM_greatswordsheath = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_greatswordsheath"));
	SM_greatswordsheath->SetupAttachment(GetMesh(), FName(TEXT("scabbard1")));
	SM_greatswordsheath->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));

	SM_greatswordsheath_HandL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_greatswordsheath_HandL"));
	SM_greatswordsheath_HandL->SetupAttachment(GetMesh(), FName(TEXT("scabbard2")));
	SM_greatswordsheath_HandL->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));

	SM_GreatSword_HandL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_GreatSword_HandL"));
	SM_GreatSword_HandL->SetupAttachment(GetMesh(), FName(TEXT("scabbard2")));
	SM_GreatSword_HandL->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));

	SM_GreatSword = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_GreatSword"));
	SM_GreatSword->SetupAttachment(GetMesh(), FName(TEXT("scabbard1")));
	SM_GreatSword->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));


}

void AMyAniCharacter_Ghost::BeginPlay()
{
	Super::BeginPlay();
	//初始检测持剑情况
	if (IsUsingSword)
	{
		//手拿着剑，准备使用
		SM_GreatSword_HandL->SetVisibility(0, 0);
		SM_greatswordsheath_HandL->SetVisibility(0, 0);
		SM_GreatSword->SetVisibility(0, 0);
		SM_GreatSword_HandR->SetVisibility(true, 0);
		SM_greatswordsheath->SetVisibility(true, 0);
	}
	else
	{
		if (LeftHandSword)
		{
			//手拿着剑，插在剑鞘里
			SM_GreatSword_HandL->SetVisibility(true, 0);
			SM_greatswordsheath_HandL->SetVisibility(true, 0);
			SM_GreatSword->SetVisibility(0, 0);
			SM_GreatSword_HandR->SetVisibility(0, 0);
			SM_greatswordsheath->SetVisibility(0, 0);
		}
		else
		{
			//剑别在腰间，插在剑鞘里
			SM_GreatSword_HandL->SetVisibility(0, 0);
			SM_greatswordsheath_HandL->SetVisibility(0, 0);
			SM_GreatSword->SetVisibility(true, 0);
			SM_GreatSword_HandR->SetVisibility(0, 0);
			SM_greatswordsheath->SetVisibility(true, 0);
		}
		
	}
}

void AMyAniCharacter_Ghost::DrawTheSword(bool UneqiupToLeftHand)
{
	//拔/收剑
	if (!IsDrawingSword)
	{
		IsDrawingSword = 1;
		if (IsUsingSword)
		{
			IsUsingSword = 0;
			if (UnEquipMontage)
			{
				PlayAnimMontage(UnEquipMontage);

				FTimerHandle TimerHandle; float delayTime =
					0.38f;//Delay
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
					{
						//Delay Execute
						if (UneqiupToLeftHand)
						{
							//手拿着剑，插在剑鞘里
							SM_GreatSword_HandL->SetVisibility(true, 0);
							SM_greatswordsheath_HandL->SetVisibility(true, 0);
							SM_GreatSword->SetVisibility(0, 0);
							SM_GreatSword_HandR->SetVisibility(0, 0);
							SM_greatswordsheath->SetVisibility(0, 0);
						}
						else
						{
							//剑别在腰间，插在剑鞘里
							SM_GreatSword_HandL->SetVisibility(0, 0);
							SM_greatswordsheath_HandL->SetVisibility(0, 0);
							SM_GreatSword->SetVisibility(true, 0);
							SM_GreatSword_HandR->SetVisibility(0, 0);
							SM_greatswordsheath->SetVisibility(true, 0);
						}
						LeftHandSword = UneqiupToLeftHand;
						IsDrawingSword = 0;

					}, delayTime, false);

			}
		}
		else
		{
			if (EquipMontage)
			{
				PlayAnimMontage(EquipMontage);

				FTimerHandle TimerHandle; float delayTime =
					0.31f;//Delay
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
					{
						//Delay Execute
						// 
						//手拿着剑，准备使用
						SM_GreatSword_HandL->SetVisibility(0, 0);
						SM_greatswordsheath_HandL->SetVisibility(0, 0);
						SM_GreatSword->SetVisibility(0, 0);
						SM_GreatSword_HandR->SetVisibility(true, 0);
						SM_greatswordsheath->SetVisibility(true, 0);

						IsDrawingSword = 0;
						IsUsingSword = 1;
					}, delayTime, false);

			}



		}
	}
}
