// Fill out your copyright notice in the Description page of Project Settings.


#include "MyLevelScriptActor.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"

/*
	关卡蓝图基类
*/

AMyLevelScriptActor::AMyLevelScriptActor()
{
	
}

void AMyLevelScriptActor::DoorOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	//玩家推门
	if (OtherActor)
	{
		if (OtherActor == PlayerPawn)
		{
			if (FVector::DotProduct(
				Door->GetActorForwardVector(),
				OtherActor->GetActorForwardVector())
				> 0.5)
			{
				//正面推门
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
	//敌人跑的时候把门关了
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
	//敌人跑的时候把凳子带上
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
	//玩家走到该区域发现危险，抽出剑
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
	//绑定场景物件的碰撞
	//场景物件要在实际关卡蓝图中设置
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
	//最初准备
	PlayerPawn->DisableInput(PC);
	PlayerPawn->HideBody();
	PlayerPawn->BindAniCharacter(Hero);
	if (PlayerPawn->IsUsingSword)
	{
		PlayerPawn->DrawTheSword();
	}

	//第一段剧情，模仿鬼泣5 最终战 的开头CG

	ChangeCamera(1);
	GhostPlayMontage(0);
	//主角走过来
	HeroMoveToD(1);
	ChangeCameraTime(2,4.0f);
	//继续向前走
	FTimerHandle TimerHandle; float delayTime =
		3.0f;//Delay
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			//Delay Execute
			HeroMoveToD(2);
			
		}, delayTime, false);

	//镜头往前
	FTimerHandle TimerHandle2;
	delayTime =
		6.0f;//Delay
			GetWorld()->GetTimerManager().SetTimer(TimerHandle2, [&]()
		{
			//Delay Execute
			ChangeCamera(3);
			
		}, delayTime, false);
	
	//特写敌人
	delayTime += 3.0f;//Delay
	FTimerHandle TimerHandle3;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle3, [&]()
		{
			//Delay Execute
			ChangeCamera(4);
			ChangeCameraTime(5,3.0f);

		}, delayTime, false);
	//敌人起身
	delayTime += 1.0f;//Delay
	FTimerHandle TimerHandle4;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle4, [&]()
		{
			//Delay Execute
			GhostPlayMontage(1);

		}, delayTime, false);
	//敌人走出座位
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

	//敌人拔刀
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
					//主角拔刀
					Hero->DrawTheSword();
					ChangeCamera(8);
					ChangeCameraTime(9, 3.0f);
				}, delayTime2, false);

			

		}, delayTime, false);

	//第一次开打
	delayTime += 4.5f;//Delay
	FTimerHandle TimerHandle7;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle7, [&]()
		{
			//Delay Execute
			ChangeCamera(10);
			GhostSetTo(3);
			HeroSetTo(2);
			//步行变成跑步速度
			Ghost->GetCharacterMovement()->MaxWalkSpeed = 600.0f;
			Hero->GetCharacterMovement()->MaxWalkSpeed = 600.0f;
			/// 靠近
			WalkClose(150.0f);

			float delayTime2 = 0.5f;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle, [&]()
				{
					//Delay Execute
					//战斗中
					UGameplayStatics::SetGlobalTimeDilation(this, 0.25f);
					GhostPlayMontage(2);
					HeroPlayMontage(3);

					float delayTime3 = 0.37f;
					GetWorld()->GetTimerManager().SetTimer(
						TimerHandle2, [&]()
						{
							//Delay Execute
							//Hero受击
							Hero->Displacement(250.0f * Ghost->GetActorForwardVector(),0.6f);
							HeroPlayMontage(4);
							GhostPlayMontage(5);
							//Ghost打第二下

							float delayTime4 = 0.30f;
							GetWorld()->GetTimerManager().SetTimer(
								TimerHandle3, [&]()
								{
									//Delay Execute
									//Hero受击
									
									HeroPlayMontage(6);
									Hero->Displacement(FVector(0),1.0f);

									float delayTime5 = 0.20f;
									GetWorld()->GetTimerManager().SetTimer(
										TimerHandle4, [&]()
										{
											//Delay Execute
											//Hero后跳闪避
										HeroPlayMontage(7);
										ChangeCamera(11);
										Hero->Displacement(-150.0f * Hero->GetActorForwardVector(), 0.5f);


										float delayTime6 = 0.50f;
										GetWorld()->GetTimerManager().SetTimer(
											TimerHandle5, [&]()
											{
												//Delay Execute
												//Ghost位移一下，准备下一战
												Ghost->Displacement(-50.0f * Ghost->GetActorForwardVector(), 0.2f);

										}, delayTime6, false);


									}, delayTime5, false);



								}, delayTime4, false);


						}, delayTime3, false);


				}, delayTime2, false);



		}, delayTime, false);







	//第一次开打的第二回合！
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
					//Ghost架防
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
									//两人弹开

									HeroPlayMontage(11);
									GhostPlayMontage(12);


								}, delayTime4, false);


						}, delayTime3, false);


				}, delayTime2, false);



		}, delayTime, false);



	//第一次开打的第三回合！
	delayTime += 1.24f;//Delay 1.24
	FTimerHandle TimerHandle9;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle9, [&]()
		{
			//Delay Execute
			//Hero转向Ghost，再来一剑
			Hero->TurnToRotation(UKismetMathLibrary::FindLookAtRotation(Hero->GetActorLocation(), Ghost->GetActorLocation()));
			HeroPlayMontage(13);

			float delayTime2 = 0.3f;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle, [&]()
				{
					//Delay Execute
					//Hero冲前
					Hero->Displacement(30.0f * UKismetMathLibrary::GetDirectionUnitVector(Hero->GetActorLocation(), Ghost->GetActorLocation())
						, 0.2f);
					ChangeCamera(14);
					GhostPlayMontage(14);

					float delayTime3 = 0.3f;
					GetWorld()->GetTimerManager().SetTimer(
						TimerHandle2, [&]()
						{
							//Delay Execute
							//Ghost受击
							GhostPlayMontage(15);

							float delayTime4 = 0.4f;
							GetWorld()->GetTimerManager().SetTimer(
								TimerHandle3, [&]()
								{
									//Delay Execute
									//Ghost侧边闪避
									GhostPlayMontage(16);


								}, delayTime4, false);


						}, delayTime3, false);


				}, delayTime2, false);



		}, delayTime, false);

	//Ghost最后假装一击！
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

					//Hero躲开

					float delayTime3 = 0.6f;
					GetWorld()->GetTimerManager().SetTimer(
						TimerHandle2, [&]()
						{
							//Delay Execute
							//Ghost收起剑逃跑
							Ghost->DrawTheSword(0);

							float delayTime4 = 0.3f;
							GetWorld()->GetTimerManager().SetTimer(
								TimerHandle3, [&]()
								{
									//Delay Execute
									//Ghost逃跑
									GhostMoveToD(5);
									//时间流速
									UGameplayStatics::SetGlobalTimeDilation(this, 0.5f);
									ChangeCameraTime(16, 1.0f);


								}, delayTime4, false);


						}, delayTime3, false);


				}, delayTime2, false);



		}, delayTime, false);




	//Ghost逃跑成功
	delayTime += 4.0f;//Delay 3.3
	FTimerHandle TimerHandle11;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle11, [&]()
		{

			Ghost->HideBody();
			Chair1->Destroy();
			//时间流速
			UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);
			//镜头回到玩家位置
			PC->SetViewTargetWithBlend(Hero,5.0f);
			Hero->DrawTheSword();

			float delayTime2 = 3.0f;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle, [&]()
				{
					//Delay Execute
					//交还控制权，自由移动
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
			//第二段剧情，模仿FF14 5.0CG的开头部分
			PlayerPawn->DrawTheSword();
			Hero->SetActorTransform(PlayerPawn->GetActorTransform());
			//玩家收起剑
			float delayTime = 0.7f;//Delay 
			FTimerHandle TimerHandle12;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle12, [&]()
				{
					ChangeCamera(17);
					PlayerPawn->SwitchToCGPawn();
					HeroSetTo(6);
					Hero->GetCharacterMovement()->MaxWalkSpeed = 100.0f;
					//Hero步行向前
					HeroMoveToD(7);
				}, delayTime, false);

			delayTime += 2.0f;//Delay 
			FTimerHandle TimerHandle13;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle13, [&]()
				{
					//玩家突然头疼，捂头
					ChangeCamera(18);
					HeroPlayMontage(19);
					//自制动作
					
				}, delayTime, false);

			delayTime += 5.5f;//Delay 
			FTimerHandle TimerHandle14;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle14, [&]()
				{
					//玩家听到下方的声音，放下手转头看（时间原因没做个动物放上去www）
					ChangeCamera(19);
				}, delayTime, false);

			delayTime += 2.5f;//Delay 
			FTimerHandle TimerHandle15;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle15, [&]()
				{
					//玩家在看着说话的动物（未实现动物）
					Hero->SetActorRotation(FRotator(0, -40.0f, 0));
					ChangeCamera(20);
					//玩家注意到身后的敌人，转头看
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
					//敌人正在天上飘下来
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
					//敌人开始降落

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
					//敌人开始降落loop
					
					GhostPlayMontage(23);

				}, delayTime, false);

			delayTime += 0.8f;//Delay 
			FTimerHandle TimerHandle19;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle19, [&]()
				{
					//敌人降落，准备开打
					Ghost->StopAnimMontage();
					Ghost->GetCharacterMovement()->GravityScale = 1.0f;
					Ghost->ShowBody();
					Hero->ShowBody();
					GhostSetTo(9);
					HeroSetTo(7);
					ChangeCamera(23);
					//时间流速
					UGameplayStatics::SetGlobalTimeDilation(this, 0.25f);
					//步行变成跑步速度
					Ghost->GetCharacterMovement()->MaxWalkSpeed = 400.0f;
					Hero->GetCharacterMovement()->MaxWalkSpeed = 400.0f;
					WalkClose(100.0f);
				}, delayTime, false);

			//决战，第一回合
			delayTime += 0.6f;//Delay 
			FTimerHandle TimerHandle20;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle20, [&]()
				{
					//启用手脚ik
					Ghost->EnableBodyIK = 1;
					Hero->EnableBodyIK = 1;
					//双方出拳
					GhostPlayMontage(24);
					HeroPlayMontage(25);

					float delayTime2 = 0.44f;//Delay 
					FTimerHandle TimerHandle12;
					GetWorld()->GetTimerManager().SetTimer(
						TimerHandle12, [&]()
						{
							//Ghost受击
							GhostPlayMontage(26);


							float delayTime3 = 0.36f;//Delay 
							FTimerHandle TimerHandle13;
							GetWorld()->GetTimerManager().SetTimer(
								TimerHandle13, [&]()
								{
									//Hero追击
									HeroPlayMontage(27);

									float delayTime4 = 0.1f;//Delay 
									FTimerHandle TimerHandle14;
									GetWorld()->GetTimerManager().SetTimer(
										TimerHandle14, [&]()
										{
											//Hero前冲
											
											Hero->Displacement(200.0f * Hero->GetActorForwardVector()
												, 0.4f);

											float delayTime5 = 0.4f;//Delay 
											FTimerHandle TimerHandle15;
											GetWorld()->GetTimerManager().SetTimer(
												TimerHandle15, [&]()
												{
													//Ghost受击
													GhostPlayMontage(28);

													float delayTime6 = 0.2f;//Delay 
													FTimerHandle TimerHandle16;
													GetWorld()->GetTimerManager().SetTimer(
														TimerHandle16, [&]()
														{
															WalkClose(60.0f);
															//靠近一点！
															HeroPlayMontage(29);
															GhostPlayMontage(30);

															float delayTime7 = 0.5f;//Delay 
															FTimerHandle TimerHandle17;
															GetWorld()->GetTimerManager().SetTimer(
																TimerHandle17, [&]()
																{
																	//Hero受击
																	Hero->Displacement(-100.0f * Hero->GetActorForwardVector()
																		, 0.4f);
																	HeroPlayMontage(31);

																	float delayTime8 = 0.55f;//Delay 0.75
																	FTimerHandle TimerHandle18;
																	GetWorld()->GetTimerManager().SetTimer(
																		TimerHandle18, [&]()
																		{
																			//Hero反击上勾拳
																			HeroPlayMontage(32);

																			float delayTime9 = 0.35f;//Delay 
																			FTimerHandle TimerHandle27;
																			GetWorld()->GetTimerManager().SetTimer(
																				TimerHandle27, [&]()
																				{
																					//Ghost被打飞
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
			
			//最终战第二回合
			delayTime += 3.4f;//Delay 3.1+0.3
			FTimerHandle TimerHandle21;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle21, [&]()
				{
					//Ghost抽出刀
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
					//双方的最终较量，拳头打剑
					GhostPlayMontage(34);
					HeroPlayMontage(35);


					float delayTime2 = 0.35f;//Delay
					FTimerHandle TimerHandle16;
					GetWorld()->GetTimerManager().SetTimer(
						TimerHandle16, [&]()
						{
							//Ghost被打倒

							//关闭Ghost手脚ik
							Ghost->EnableBodyIK = 0;
							//Hero->EnableBodyIK = 0;
							GhostPlayMontage(36);

							
							float delayTime3 = 1.25f;//Delay
							FTimerHandle TimerHandle17;
							GetWorld()->GetTimerManager().SetTimer(
								TimerHandle17, [&]()
								{
									//Ghost开始躺地上
									GhostPlayMontage(37);
									ChangeCamera(25);

									float delayTime4 = 0.5f;//Delay
									FTimerHandle TimerHandle18;
									GetWorld()->GetTimerManager().SetTimer(
										TimerHandle18, [&]()
										{
											//Hero拿出剑
											Hero->DrawTheSword();

											float delayTime5 = 0.5f;//Delay
											FTimerHandle TimerHandle19;
											GetWorld()->GetTimerManager().SetTimer(
												TimerHandle19, [&]()
												{
													//凳子来了！
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
															//凳子带走主人
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
																	//最终运镜
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
	//换摄像机（有绑定时间）
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
	//二人靠近，到一定距离停下
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
	//把Ghost设置到指定id的TargetPoint位置
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
	//把Ghost设置到指定id的TargetPoint位置
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

