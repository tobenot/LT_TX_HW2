// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWine.h"
#include "MyCharacter.h"
#include "Kismet/KismetMathLibrary.h"

/*
	ƿ��
	���������˿��Ի�Ѫ
*/
AMyWine::AMyWine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SphereComp->SetupAttachment(MeshComp);
	SphereComp->SetSphereRadius(64.0f);
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AMyWine::SphereBeginOverlap);
	RootComponent = SphereComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	MeshComp->SetupAttachment(RootComponent);

	
}

// Called when the game starts or when spawned
void AMyWine::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyWine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyWine::SphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//��������˼����ʰȡ
	AMyCharacter* Pawn = Cast<AMyCharacter>(OtherActor);
	if (Pawn)
	{
		if (!BeDrink)
		{
			BeDrink = 1;
			/*FRotator Target = UKismetMathLibrary::FindLookAtRotation(Pawn->GetActorLocation(), GetActorLocation());
			Target.Pitch = 0; 
			Target.Roll = 0;
			Pawn->SetActorRotation(Target);*/
			Pawn->PickUpAni();
			if (Pawn)
			{
				Pawn->Heal(25.0f);
				//����Ž�delay���棬���޷��������У����һ��
			}
			FTimerHandle TimerHandle; float delayTime =
				0.35f;//Delay
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
				{
					//Delay Execute
					
					if (MeshComp)
					{
						//�����Ҷ�����ʧ
						MeshComp->SetVisibility(0, 0);
						FTimerHandle TimerHandle; float delayTime =
							0.25f;//Delay
						GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
							{
								//Delay Execute
								
								Destroy();

							}, delayTime, false);
					}

				}, delayTime, false);
		}
	}
}