// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyCharacter.h"

/*
	飞来的敌人
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
	// 组件击中某物时调用的事件。
	MeshComp->OnComponentHit.AddDynamic(this, &AMyEnemy::OnHit);

	//敌人的初始值，子类有不同的值
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
	//获得玩家角色
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
			//如果玩家离得够近，就向玩家加速
			if (Length <= AttackDistance)
			{
				ProjectileMovementComponent->Activate();
				if (!HaveStartAttack)
				{
					//首次攻击会飞起来
					HaveStartAttack = 1;
					StartAttack();
				}

				(Pawn->GetActorLocation() - GetActorLocation() - FVector(0,0,100)).ToDirectionAndLength(Direction, Length);

				ProjectileMovementComponent->Velocity = ProjectileMovementComponent->Velocity + DeltaTime * FlySpeed * Direction; 
				FVector DirectionSpeed;
				ProjectileMovementComponent->Velocity.ToDirectionAndLength(DirectionSpeed, Length);
				
				//如果飞过头了，就很快地减速
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
	//首次攻击，向上弹射
	ProjectileMovementComponent->Velocity = ProjectileMovementComponent->Velocity + FVector(0, 0, UpLaunchSpeed);

	FTimerHandle TimerHandle; float delayTime =
		0.5f;//Delay
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			//Delay Execute
			//过一小点时间后把弹射速度减掉
			ProjectileMovementComponent->Velocity = ProjectileMovementComponent->Velocity - FVector(0, 0, UpLaunchSpeed);
		}, delayTime, false);

}

void AMyEnemy::DeadPhy()
{
	//变成物理体
	MeshComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	MeshComp->SetSimulatePhysics(1);
	MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);

}

void AMyEnemy::SwordSlice()
{
	//被玩家击中
	IsDead = 1;
	DeadPhy();
	//高速旋转，显示是被玩家击中的
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
				//如果是敌人主动撞到了玩家，就对玩家造成伤害
				PlayerPawn->TakeDamageFromMovingEnemy(ImpulseMup * ProjectileMovementComponent->Velocity, Damage);
				
			}
			DeadPhy();
		}
	}
	

}