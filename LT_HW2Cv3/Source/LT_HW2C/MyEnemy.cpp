// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyCharacter.h"

/*
	�����ĵ���
*/
AMyEnemy::AMyEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;


	if (!ProjectileMovementComponent)
	{
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(MeshComp);
		ProjectileMovementComponent->InitialSpeed = 0.0f;
		ProjectileMovementComponent->MaxSpeed = 0.0f;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = false;
		ProjectileMovementComponent->bShouldBounce = false;
		ProjectileMovementComponent->Bounciness = 0.3f;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
		ProjectileMovementComponent->bAutoActivate = 0;
	}
	// �������ĳ��ʱ���õ��¼���
	MeshComp->OnComponentHit.AddDynamic(this, &AMyEnemy::OnHit);

	//���˵ĳ�ʼֵ�������в�ͬ��ֵ
	AttackDistance = 1200.0f;
	UpLaunchSpeed=400.0f;
	FlySpeed=800.0f;
	ImpulseMup = 0.2f;
	Damage=30.0f;

}

// Called when the game starts or when spawned
void AMyEnemy::BeginPlay()
{
	Super::BeginPlay();
	//�����ҽ�ɫ
	Pawn = (UGameplayStatics::GetPlayerPawn(this, 0));

}

// Called every frame
void AMyEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!IsDead)
	{
		if (Pawn)
		{
			FVector Direction;
			float Length;
			//Vector.ToDirectionAndLength(Direction, Length);
			(Pawn->GetActorLocation() - GetActorLocation()).ToDirectionAndLength(Direction, Length);
			//��������ù�����������Ҽ���
			if (Length <= AttackDistance)
			{
				ProjectileMovementComponent->Activate();
				if (!HaveStartAttack)
				{
					//�״ι����������
					HaveStartAttack = 1;
					StartAttack();
				}

				(Pawn->GetActorLocation() - GetActorLocation() - FVector(0,0,100)).ToDirectionAndLength(Direction, Length);

				ProjectileMovementComponent->Velocity = ProjectileMovementComponent->Velocity + DeltaTime * FlySpeed * Direction; 
				FVector DirectionSpeed;
				ProjectileMovementComponent->Velocity.ToDirectionAndLength(DirectionSpeed, Length);
				
				//����ɹ�ͷ�ˣ��ͺܿ�ؼ���
				if (FVector::DotProduct(Direction, DirectionSpeed) < 0)
				{
					ProjectileMovementComponent->Velocity = ProjectileMovementComponent->Velocity * (0.5 * DeltaTime);
				}


			}
		}
	}
}

void AMyEnemy::StartAttack()
{
	//�״ι��������ϵ���
	ProjectileMovementComponent->Velocity = ProjectileMovementComponent->Velocity + FVector(0, 0, UpLaunchSpeed);

	FTimerHandle TimerHandle; float delayTime =
		0.5f;//Delay
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			//Delay Execute
			//��һС��ʱ���ѵ����ٶȼ���
			ProjectileMovementComponent->Velocity = ProjectileMovementComponent->Velocity - FVector(0, 0, UpLaunchSpeed);
		}, delayTime, false);

}

void AMyEnemy::DeadPhy()
{
	//���������
	MeshComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	MeshComp->SetSimulatePhysics(1);
	MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);

}

void AMyEnemy::SwordSlice()
{
	//����һ���
	IsDead = 1;
	DeadPhy();
	//������ת����ʾ�Ǳ���һ��е�
	MeshComp->AddAngularImpulseInRadians(UKismetMathLibrary::RandomUnitVector() * 50.0f, NAME_None, 1);;

}


void AMyEnemy::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!IsDead)
	{
		if (OtherActor == Pawn)
		{
			IsDead = 1;
			AMyCharacter* PlayerPawn = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
			if (PlayerPawn)
			{
				//����ǵ�������ײ������ң��Ͷ��������˺�
				PlayerPawn->TakeDamageFromMovingEnemy(ImpulseMup * ProjectileMovementComponent->Velocity, Damage);
				
			}
			DeadPhy();
		}
	}
	

}