// Fill out your copyright notice in the Description page of Project Settings.


#include "MyLevelScriptActor.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"

/*
	�ؿ���ͼ����
*/

AMyLevelScriptActor::AMyLevelScriptActor()
{
	
}

void AMyLevelScriptActor::DoorOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	//�������
	if (OtherActor)
	{
		if (OtherActor == PlayerPawn)
		{
			if (FVector::DotProduct(
				Door->GetActorForwardVector(),
				OtherActor->GetActorForwardVector())
				> 0.5)
			{
				//��������
				FRotator tp = 
				UKismetMathLibrary::ComposeRotators(Door->GetActorRotation(),
					FRotator(0, -20.0f * UGameplayStatics::GetWorldDeltaSeconds(this), 0));

				tp.Yaw = tp.Yaw > 90.0f ? 90.0f : tp.Yaw < 0 ? 0 : tp.Yaw;
				Door->SetActorRotation(tp);
			}

		}
	}
	
}

void AMyLevelScriptActor::TriggerBox1_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//�����ܵ�ʱ����Ź���
	if (OtherActor)
	{
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;
		UKismetSystemLibrary::MoveComponentTo(
			Door->GetStaticMeshComponent(),
			Door->GetActorLocation(),
			UKismetMathLibrary::ComposeRotators(Door->GetActorRotation(),
				FRotator(0, 90.0f, 0)), 0, 0, 0.5f, 0,EMoveComponentAction::Move, LatentInfo
		);
		TriggerBoxs[1]->SetActorEnableCollision(0);
	}
}

void AMyLevelScriptActor::TriggerBox2_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//�����ܵ�ʱ��ѵ��Ӵ���
	if (OtherActor)
	{
		if (OtherActor == Ghost)
		{
			Chair1->AttachToComponent(Ghost->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), "ChairSocket");
		}
	}
}

void AMyLevelScriptActor::TriggerBox3_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//����ߵ���������Σ�գ������
	if (OtherActor)
	{
		if (OtherActor == PlayerPawn)
		{
			PlayerPawn->DrawTheSword();
			TriggerBoxs[3]->SetActorEnableCollision(0);
		}
	}
}


void AMyLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();
	//�󶨳����������ײ
	//�������Ҫ��ʵ�ʹؿ���ͼ������
	if (Door)
	{
		Door->GetStaticMeshComponent()->OnComponentHit.AddDynamic(this, &AMyLevelScriptActor::DoorOnHit);
	}
	if (TriggerBoxs.IsValidIndex(1))
	{
		TriggerBoxs[1]->GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMyLevelScriptActor::TriggerBox1_BeginOverlap);
	}
	if (TriggerBoxs.IsValidIndex(2))
	{
		TriggerBoxs[2]->GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMyLevelScriptActor::TriggerBox2_BeginOverlap);
	}
	if (TriggerBoxs.IsValidIndex(3))
	{
		TriggerBoxs[3]->GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMyLevelScriptActor::TriggerBox3_BeginOverlap);
	}
	if (TriggerBoxs.IsValidIndex(4))
	{
		TriggerBoxs[4]->GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMyLevelScriptActor::TriggerBox4_BeginOverlap);
	}
	HeroC = Cast<AAIController>(Hero->GetController());
	GhostC = Cast<AAIController>(Ghost->GetController());
	PlayerPawn = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	PC = UGameplayStatics::GetPlayerController(this, 0);
	//���׼��
	PlayerPawn->DisableInput(PC);
	PlayerPawn->HideBody();
	PlayerPawn->BindAniCharacter(Hero);
	if (PlayerPawn->IsUsingSword)
	{
		PlayerPawn->DrawTheSword();
	}

	//��һ�ξ��飬ģ�¹���5 ����ս �Ŀ�ͷCG

	ChangeCamera(1);
	GhostPlayMontage(0);
	//�����߹���
	HeroMoveToD(1);
	ChangeCameraTime(2,4.0f);
	//������ǰ��
	FTimerHandle TimerHandle; float delayTime =
		3.0f;//Delay
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			//Delay Execute
			HeroMoveToD(2);
			
		}, delayTime, false);

	//��ͷ��ǰ
	FTimerHandle TimerHandle2;
	delayTime =
		6.0f;//Delay
			GetWorld()->GetTimerManager().SetTimer(TimerHandle2, [&]()
		{
			//Delay Execute
			ChangeCamera(3);
			
		}, delayTime, false);
	
	//��д����
	delayTime += 3.0f;//Delay
	FTimerHandle TimerHandle3;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle3, [&]()
		{
			//Delay Execute
			ChangeCamera(4);
			ChangeCameraTime(5,3.0f);

		}, delayTime, false);
	//��������
	delayTime += 1.0f;//Delay
	FTimerHandle TimerHandle4;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle4, [&]()
		{
			//Delay Execute
			GhostPlayMontage(1);

		}, delayTime, false);
	//�����߳���λ
	delayTime += 1.5f;//Delay
	FTimerHandle TimerHandle5;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle5, [&]()
		{
			//Delay Execute
			GhostSetTo(4);
			Ghost->StopAnimMontage();
			Ghost->TurnToRotation(FRotator(0,-130.0f,0));
			GhostMoveToD(3);
			ChangeCamera(6);
			ChangeCameraTime(7,3.0f);

		}, delayTime, false);

	//���˰ε�
	delayTime += 1.5f;//Delay
	FTimerHandle TimerHandle6;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle6, [&]()
		{
			//Delay Execute
			Ghost->DrawTheSword(0);
			float delayTime2 = 1.5f;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle, [&]()
				{
					//Delay Execute
					//���ǰε�
					Hero->DrawTheSword();
					ChangeCamera(8);
					ChangeCameraTime(9, 3.0f);
				}, delayTime2, false);

			

		}, delayTime, false);

	//��һ�ο���
	delayTime += 4.5f;//Delay
	FTimerHandle TimerHandle7;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle7, [&]()
		{
			//Delay Execute
			ChangeCamera(10);
			GhostSetTo(3);
			HeroSetTo(2);
			//���б���ܲ��ٶ�
			Ghost->GetCharacterMovement()->MaxWalkSpeed = 600.0f;
			Hero->GetCharacterMovement()->MaxWalkSpeed = 600.0f;
			/// ����
			WalkClose(150.0f);

			float delayTime2 = 0.5f;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle, [&]()
				{
					//Delay Execute
					//ս����
					UGameplayStatics::SetGlobalTimeDilation(this, 0.25f);
					GhostPlayMontage(2);
					HeroPlayMontage(3);

					float delayTime3 = 0.37f;
					GetWorld()->GetTimerManager().SetTimer(
						TimerHandle2, [&]()
						{
							//Delay Execute
							//Hero�ܻ�
							Hero->Displacement(250.0f * Ghost->GetActorForwardVector(),0.6f);
							HeroPlayMontage(4);
							GhostPlayMontage(5);
							//Ghost��ڶ���

							float delayTime4 = 0.30f;
							GetWorld()->GetTimerManager().SetTimer(
								TimerHandle3, [&]()
								{
									//Delay Execute
									//Hero�ܻ�
									
									HeroPlayMontage(6);
									Hero->Displacement(FVector(0),1.0f);

									float delayTime5 = 0.20f;
									GetWorld()->GetTimerManager().SetTimer(
										TimerHandle4, [&]()
										{
											//Delay Execute
											//Hero��������
										HeroPlayMontage(7);
										ChangeCamera(11);
										Hero->Displacement(-150.0f * Hero->GetActorForwardVector(), 0.5f);


										float delayTime6 = 0.50f;
										GetWorld()->GetTimerManager().SetTimer(
											TimerHandle5, [&]()
											{
												//Delay Execute
												//Ghostλ��һ�£�׼����һս
												Ghost->Displacement(-50.0f * Ghost->GetActorForwardVector(), 0.2f);

										}, delayTime6, false);


									}, delayTime5, false);



								}, delayTime4, false);


						}, delayTime3, false);


				}, delayTime2, false);



		}, delayTime, false);







	//��һ�ο���ĵڶ��غϣ�
	delayTime += 1.9f;//Delay 2.17
	FTimerHandle TimerHandle8;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle8, [&]()
		{
			//Delay Execute
			HeroPlayMontage(8);
			ChangeCamera(12);

			float delayTime2 = 0.22f;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle, [&]()
				{
					//Delay Execute
					//Ghost�ܷ�
					GhostPlayMontage(9);
					ChangeCamera(13);

					float delayTime3 = 0.12f;
					GetWorld()->GetTimerManager().SetTimer(
						TimerHandle2, [&]()
						{
							//Delay Execute
							//
							GhostPlayMontage(10);

							float delayTime4 = 0.4f;
							GetWorld()->GetTimerManager().SetTimer(
								TimerHandle3, [&]()
								{
									//Delay Execute
									//���˵���

									HeroPlayMontage(11);
									GhostPlayMontage(12);


								}, delayTime4, false);


						}, delayTime3, false);


				}, delayTime2, false);



		}, delayTime, false);



	//��һ�ο���ĵ����غϣ�
	delayTime += 1.24f;//Delay 1.24
	FTimerHandle TimerHandle9;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle9, [&]()
		{
			//Delay Execute
			//Heroת��Ghost������һ��
			Hero->TurnToRotation(UKismetMathLibrary::FindLookAtRotation(Hero->GetActorLocation(), Ghost->GetActorLocation()));
			HeroPlayMontage(13);

			float delayTime2 = 0.3f;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle, [&]()
				{
					//Delay Execute
					//Hero��ǰ
					Hero->Displacement(30.0f * UKismetMathLibrary::GetDirectionUnitVector(Hero->GetActorLocation(), Ghost->GetActorLocation())
						, 0.2f);
					ChangeCamera(14);
					GhostPlayMontage(14);

					float delayTime3 = 0.3f;
					GetWorld()->GetTimerManager().SetTimer(
						TimerHandle2, [&]()
						{
							//Delay Execute
							//Ghost�ܻ�
							GhostPlayMontage(15);

							float delayTime4 = 0.4f;
							GetWorld()->GetTimerManager().SetTimer(
								TimerHandle3, [&]()
								{
									//Delay Execute
									//Ghost�������
									GhostPlayMontage(16);


								}, delayTime4, false);


						}, delayTime3, false);


				}, delayTime2, false);



		}, delayTime, false);

	//Ghost����װһ����
	delayTime += 1.9f;//Delay 1.6
	FTimerHandle TimerHandle10;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle10, [&]()
		{
			ChangeCamera(15);
			WalkClose(50.0f);
			float delayTime2 = 0.3f;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle, [&]()
				{
					//Delay Execute
					//
					GhostPlayMontage(17);
					HeroPlayMontage(18);

					//Hero�㿪

					float delayTime3 = 0.6f;
					GetWorld()->GetTimerManager().SetTimer(
						TimerHandle2, [&]()
						{
							//Delay Execute
							//Ghost��������
							Ghost->DrawTheSword(0);

							float delayTime4 = 0.3f;
							GetWorld()->GetTimerManager().SetTimer(
								TimerHandle3, [&]()
								{
									//Delay Execute
									//Ghost����
									GhostMoveToD(5);
									//ʱ������
									UGameplayStatics::SetGlobalTimeDilation(this, 0.5f);
									ChangeCameraTime(16, 1.0f);


								}, delayTime4, false);


						}, delayTime3, false);


				}, delayTime2, false);



		}, delayTime, false);




	//Ghost���ܳɹ�
	delayTime += 4.0f;//Delay 3.3
	FTimerHandle TimerHandle11;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle11, [&]()
		{

			Ghost->HideBody();
			Chair1->Destroy();
			//ʱ������
			UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);
			//��ͷ�ص����λ��
			PC->SetViewTargetWithBlend(Hero,5.0f);
			Hero->DrawTheSword();

			float delayTime2 = 3.0f;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle, [&]()
				{
					//Delay Execute
					//��������Ȩ�������ƶ�
					PlayerPawn->SwitchToControlPawn();

				}, delayTime2, false);



		}, delayTime, false);
		
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Walkin"));
}

void AMyLevelScriptActor::TriggerBox4_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (OtherActor == PlayerPawn)
		{
			//�ڶ��ξ��飬ģ��FF14 5.0CG�Ŀ�ͷ����
			PlayerPawn->DrawTheSword();
			Hero->SetActorTransform(PlayerPawn->GetActorTransform());
			//�������
			float delayTime = 0.7f;//Delay 
			FTimerHandle TimerHandle12;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle12, [&]()
				{
					ChangeCamera(17);
					PlayerPawn->SwitchToCGPawn();
					HeroSetTo(6);
					Hero->GetCharacterMovement()->MaxWalkSpeed = 100.0f;
					//Hero������ǰ
					HeroMoveToD(7);
				}, delayTime, false);

			delayTime += 2.0f;//Delay 
			FTimerHandle TimerHandle13;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle13, [&]()
				{
					//���ͻȻͷ�ۣ���ͷ
					ChangeCamera(18);
					HeroPlayMontage(19);
					//���ƶ���
					
				}, delayTime, false);

			delayTime += 5.5f;//Delay 
			FTimerHandle TimerHandle14;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle14, [&]()
				{
					//��������·���������������תͷ����ʱ��ԭ��û�����������ȥwww��
					ChangeCamera(19);
				}, delayTime, false);

			delayTime += 2.5f;//Delay 
			FTimerHandle TimerHandle15;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle15, [&]()
				{
					//����ڿ���˵���Ķ��δʵ�ֶ��
					Hero->SetActorRotation(FRotator(0, -40.0f, 0));
					ChangeCamera(20);
					//���ע�⵽���ĵ��ˣ�תͷ��
					HeroPlayMontage(20);
					Ghost->GetCharacterMovement()->GravityScale = 0.01;
					GhostSetTo(8);
					Ghost->SetActorRotation(FRotator(0, 0, 0));
				}, delayTime, false);

			delayTime += 1.4f;//Delay 
			FTimerHandle TimerHandle16;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle16, [&]()
				{
					//������������Ʈ����
					GhostPlayMontage(21);
					Ghost->SetActorHiddenInGame(0);
					ChangeCamera(21);

					/*FLatentActionInfo LatentInfo;
					LatentInfo.CallbackTarget = this;
					UKismetSystemLibrary::MoveComponentTo(
						Ghost->GetMesh(),
						FVector(200.0f,0,-200.0f),
						Ghost->GetMesh()->GetRelativeRotation(), 
						 0, 0, 1.0f, 0, EMoveComponentAction::Move, LatentInfo
					);*/

				}, delayTime, false);


			delayTime += 2.5f;//Delay 
			FTimerHandle TimerHandle17;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle17, [&]()
				{
					//���˿�ʼ����

					/*Ghost->GetMesh()->SetRelativeLocationAndRotation(
						FVector(0, 0, -90.0f),
						FRotator(0, 0, -90.0f)
					);*/
					GhostPlayMontage(22);
					ChangeCamera(22);

				}, delayTime, false);

			delayTime += 0.3f;//Delay 
			FTimerHandle TimerHandle18;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle18, [&]()
				{
					//���˿�ʼ����loop
					
					GhostPlayMontage(23);

				}, delayTime, false);

			delayTime += 0.8f;//Delay 
			FTimerHandle TimerHandle19;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle19, [&]()
				{
					//���˽��䣬׼������
					Ghost->StopAnimMontage();
					Ghost->GetCharacterMovement()->GravityScale = 1.0f;
					Ghost->ShowBody();
					Hero->ShowBody();
					GhostSetTo(9);
					HeroSetTo(7);
					ChangeCamera(23);
					//ʱ������
					UGameplayStatics::SetGlobalTimeDilation(this, 0.25f);
					//���б���ܲ��ٶ�
					Ghost->GetCharacterMovement()->MaxWalkSpeed = 400.0f;
					Hero->GetCharacterMovement()->MaxWalkSpeed = 400.0f;
					WalkClose(100.0f);
				}, delayTime, false);

			//��ս����һ�غ�
			delayTime += 0.6f;//Delay 
			FTimerHandle TimerHandle20;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle20, [&]()
				{
					//�����ֽ�ik
					Ghost->EnableBodyIK = 1;
					Hero->EnableBodyIK = 1;
					//˫����ȭ
					GhostPlayMontage(24);
					HeroPlayMontage(25);

					float delayTime2 = 0.44f;//Delay 
					FTimerHandle TimerHandle12;
					GetWorld()->GetTimerManager().SetTimer(
						TimerHandle12, [&]()
						{
							//Ghost�ܻ�
							GhostPlayMontage(26);


							float delayTime3 = 0.36f;//Delay 
							FTimerHandle TimerHandle13;
							GetWorld()->GetTimerManager().SetTimer(
								TimerHandle13, [&]()
								{
									//Hero׷��
									HeroPlayMontage(27);

									float delayTime4 = 0.1f;//Delay 
									FTimerHandle TimerHandle14;
									GetWorld()->GetTimerManager().SetTimer(
										TimerHandle14, [&]()
										{
											//Heroǰ��
											
											Hero->Displacement(200.0f * Hero->GetActorForwardVector()
												, 0.4f);

											float delayTime5 = 0.4f;//Delay 
											FTimerHandle TimerHandle15;
											GetWorld()->GetTimerManager().SetTimer(
												TimerHandle15, [&]()
												{
													//Ghost�ܻ�
													GhostPlayMontage(28);

													float delayTime6 = 0.2f;//Delay 
													FTimerHandle TimerHandle16;
													GetWorld()->GetTimerManager().SetTimer(
														TimerHandle16, [&]()
														{
															WalkClose(60.0f);
															//����һ�㣡
															HeroPlayMontage(29);
															GhostPlayMontage(30);

															float delayTime7 = 0.5f;//Delay 
															FTimerHandle TimerHandle17;
															GetWorld()->GetTimerManager().SetTimer(
																TimerHandle17, [&]()
																{
																	//Hero�ܻ�
																	Hero->Displacement(-100.0f * Hero->GetActorForwardVector()
																		, 0.4f);
																	HeroPlayMontage(31);

																	float delayTime8 = 0.55f;//Delay 0.75
																	FTimerHandle TimerHandle18;
																	GetWorld()->GetTimerManager().SetTimer(
																		TimerHandle18, [&]()
																		{
																			//Hero�����Ϲ�ȭ
																			HeroPlayMontage(32);

																			float delayTime9 = 0.35f;//Delay 
																			FTimerHandle TimerHandle27;
																			GetWorld()->GetTimerManager().SetTimer(
																				TimerHandle27, [&]()
																				{
																					//Ghost�����
																					GhostPlayMontage(33);
																					Ghost->Displacement(-150.0f * Ghost->GetActorForwardVector() - 20.0f * Ghost->GetActorRightVector()
																						, 0.2f);


																				}, delayTime9, false);


																		}, delayTime8, false);
																	


																}, delayTime7, false);

														}, delayTime6, false);


												}, delayTime5, false);


										}, delayTime4, false);


								}, delayTime3, false);

						}, delayTime2, false);

				}, delayTime, false);
			
			//����ս�ڶ��غ�
			delayTime += 3.4f;//Delay 3.1+0.3
			FTimerHandle TimerHandle21;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle21, [&]()
				{
					//Ghost�����
					Ghost->DrawTheSword(0);
					ChangeCamera(24);
				}, delayTime, false);

			delayTime += 0.4f;//Delay
			FTimerHandle TimerHandle22;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle22, [&]()
				{
					WalkClose(150.0f);
				}, delayTime, false);
				
			delayTime += 0.4f;//Delay
			FTimerHandle TimerHandle23;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle23, [&]()
				{
					//˫�������ս�����ȭͷ��
					GhostPlayMontage(34);
					HeroPlayMontage(35);


					float delayTime2 = 0.35f;//Delay
					FTimerHandle TimerHandle16;
					GetWorld()->GetTimerManager().SetTimer(
						TimerHandle16, [&]()
						{
							//Ghost����

							//�ر�Ghost�ֽ�ik
							Ghost->EnableBodyIK = 0;
							//Hero->EnableBodyIK = 0;
							GhostPlayMontage(36);

							
							float delayTime3 = 1.25f;//Delay
							FTimerHandle TimerHandle17;
							GetWorld()->GetTimerManager().SetTimer(
								TimerHandle17, [&]()
								{
									//Ghost��ʼ�ɵ���
									GhostPlayMontage(37);
									ChangeCamera(25);

									float delayTime4 = 0.5f;//Delay
									FTimerHandle TimerHandle18;
									GetWorld()->GetTimerManager().SetTimer(
										TimerHandle18, [&]()
										{
											//Hero�ó���
											Hero->DrawTheSword();

											float delayTime5 = 0.5f;//Delay
											FTimerHandle TimerHandle19;
											GetWorld()->GetTimerManager().SetTimer(
												TimerHandle19, [&]()
												{
													//�������ˣ�
													Chair2->GetStaticMeshComponent()->SetVisibility(1, 0);

													FLatentActionInfo LatentInfo;
													LatentInfo.CallbackTarget = this;
													UKismetSystemLibrary::MoveComponentTo(
														Chair2->GetStaticMeshComponent(),
														TargetPointss[10]->GetActorLocation(),
														Chair2->GetStaticMeshComponent()->GetRelativeRotation()
														, 0, 0, 0.5f, 0, EMoveComponentAction::Move, LatentInfo
													);


													float delayTime6 = 0.5f;//Delay
													FTimerHandle TimerHandle20;
													GetWorld()->GetTimerManager().SetTimer(
														TimerHandle20, [&]()
														{
															//���Ӵ�������
															Ghost->SetActorHiddenInGame(true);
															Ghost->GetCharacterMovement()->GravityScale = 0;

															FLatentActionInfo LatentInfo2;
															LatentInfo2.CallbackTarget = this;
															UKismetSystemLibrary::MoveComponentTo(
																Chair2->GetStaticMeshComponent(),
																TargetPointss[11]->GetActorLocation(),
																Chair2->GetStaticMeshComponent()->GetRelativeRotation()
																, 0, 0, 3.0f, 0, EMoveComponentAction::Move, LatentInfo2
															);

															float delayTime7 = 0.2f;//Delay
															FTimerHandle TimerHandle21;
															GetWorld()->GetTimerManager().SetTimer(
																TimerHandle21, [&]()
																{
																	//�����˾�
																	Ghost->SetActorLocation(FVector(28.0f, 46.0f, 93.0f));
																	Ghost->SetActorRotation(FRotator(0, 80.0f, 0));
																
																	Ghost->AttachToComponent(Chair2->GetStaticMeshComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), "ChairSocket");
																	Ghost->SetActorHiddenInGame(false);
																	GhostPlayMontage(0);

																	ChangeCamera(26);
																}, delayTime7, false);

														}, delayTime6, false);


												}, delayTime5, false);


										}, delayTime4, false);

								}, delayTime3, false);


						}, delayTime2, false);

				}, delayTime, false);
		}
	}
}


void AMyLevelScriptActor::ChangeCamera(int id)
{
	if (Camerass.IsValidIndex(id))
	{
		if (Camerass[id])
		{
			PC->SetViewTargetWithBlend(Camerass[id]);
		}
		
	}
		
}
void AMyLevelScriptActor::ChangeCameraTime(int id, float time)
{
	//����������а�ʱ�䣩
	if (Camerass.IsValidIndex(id))
	{
		if (Camerass[id])
		{
			PC->SetViewTargetWithBlend(Camerass[id],time);
		}

	}

}
void AMyLevelScriptActor::GhostPlayMontage(int id)
{
	if (Ghost)
	{
		if (AllCGMontages.IsValidIndex(id))
		{
			if (AllCGMontages[id])
			{
				Ghost->PlayAnimMontage(AllCGMontages[id]);
			}
		}
	}
}
void AMyLevelScriptActor::HeroPlayMontage(int id)
{
	if (Hero)
	{
		if (AllCGMontages.IsValidIndex(id))
		{
			if (AllCGMontages[id])
			{
				Hero->PlayAnimMontage(AllCGMontages[id]);
			}
		}
	}
}


void AMyLevelScriptActor::GhostMoveTo(int id, float dis)
{
	if (GhostC)
	{
		if (TargetPointss.IsValidIndex(id))
		{
			if (TargetPointss[id])
			{
				GhostC->MoveToActor(TargetPointss[id], dis);
			}
		}

	}

}

void AMyLevelScriptActor::HeroMoveTo(int id, float dis)
{
	if (HeroC)
	{
		if (TargetPointss.IsValidIndex(id))
		{
			if (TargetPointss[id])
			{
				HeroC->MoveToActor(TargetPointss[id],dis);
			}
		}
		
	}
}
void AMyLevelScriptActor::GhostMoveToD(int id)
{
	//Ghost Move to Target Point Directly
	if (GhostC)
	{
		if (TargetPointss.IsValidIndex(id))
		{
			if (TargetPointss[id])
			{
				GhostC->MoveToActor(TargetPointss[id]);
			}
		}

	}

}

void AMyLevelScriptActor::HeroMoveToD(int id)
{
	//Hero Move to Target Point Directly
	if (HeroC)
	{
		if (TargetPointss.IsValidIndex(id))
		{
			if (TargetPointss[id])
			{
				HeroC->MoveToActor(TargetPointss[id]);
			}
		}

	}
}

void AMyLevelScriptActor::WalkClose(float dis)
{
	//���˿�������һ������ͣ��
	if (HeroC)
	{
		if (Ghost)
		{
			HeroC->MoveToActor(Ghost, dis);
		}

	}
	if (GhostC)
	{
		if (Hero)
		{
			GhostC->MoveToActor(Hero, dis);
		}

	}
}

void AMyLevelScriptActor::GhostSetTo(int id)
{
	//��Ghost���õ�ָ��id��TargetPointλ��
	if (Ghost)
	{
		if (TargetPointss.IsValidIndex(id))
		{
			if (TargetPointss[id])
			{
				Ghost->SetActorLocation(TargetPointss[id]->GetActorLocation());
			}
		}

	}

}

void AMyLevelScriptActor::HeroSetTo(int id)
{
	//��Ghost���õ�ָ��id��TargetPointλ��
	if (Hero)
	{
		if (TargetPointss.IsValidIndex(id))
		{
			if (TargetPointss[id])
			{
				Hero->SetActorLocation(TargetPointss[id]->GetActorLocation());
			}
		}

	}
}

