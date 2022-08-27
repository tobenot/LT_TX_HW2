// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance_Hero.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

/*
	主角的动画蓝图基类
*/
void UMyAnimInstance_Hero::CPPUpdateAnimation(float DeltaTimeX)
{
	APawn* Owner = TryGetPawnOwner();
	if (Owner)
	{
		IsInAir = Owner->GetMovementComponent()->IsFalling();
		Speed = Owner->GetVelocity().Size();
		Direction = CalculateDirection(Owner->GetVelocity(), Owner->GetBaseAimRotation()); 

		//计算角色看向
		FRotator tp = UKismetMathLibrary::NormalizedDeltaRotator(Owner->GetControlRotation(), Owner->GetActorRotation());;
		FRotator facetp = FRotator(UKismetMathLibrary::ClampAngle(tp.Pitch, -90, 90), 
			UKismetMathLibrary::ClampAngle(tp.Yaw, -90, 90), 0);
		FRotator intp = UKismetMathLibrary::RInterpTo(FRotator(Pitch, Yaw, 0), facetp, DeltaTimeX, 3.0f);
		Pitch = intp.Pitch;
		Yaw = intp.Yaw;

	}
	if (!IsAniCharacter)
	{
		if (!PlayerPawn)
		{
			PlayerPawn = Cast<AMyCharacter>(Owner);
		}
		if (PlayerPawn)
		{
			if (PlayerPawn->LeftHandIK)
			{
				//左手摸门IK 射线检测
				
				const FVector Start = GetOwningComponent()->GetSocketLocation("spine_02");
				const FVector End = Start + (GetOwningComponent()->GetRightVector() * (0.8f) +
					GetOwningComponent()->GetForwardVector() * (0.2f)) * 80.0f;
				const float TraceRadius = 10.0f;
				const bool IgnoreSelf = 1;
				//不要检测的actors
				TArray<AActor*> ActorsToIgnore;
				ActorsToIgnore.Add(TryGetPawnOwner());
				//检查的结果
				FHitResult HitResult;
				//检测的结果
				const bool Hit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, TraceRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility)
					, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, IgnoreSelf,
					FLinearColor::Red, FLinearColor::Green, 5.0f);
				
				//摸门的IK在开始和结束都要过渡
				if (Hit)
				{
					LeftHandInterEffectorLocation = HitResult.Location + FVector(0, 0, -15.0f) + GetOwningComponent()->GetRightVector() * (2.0f);
					LeftHandInterAlpha = LeftHandInterAlpha + DeltaTimeX * 5.0f;
					LeftHandInterAlpha = LeftHandInterAlpha > 1 ? 1 : (LeftHandInterAlpha < 0 ? 0 : LeftHandInterAlpha);
					
				}else{
					LeftHandInterAlpha = LeftHandInterAlpha + DeltaTimeX * -5.0f;
					LeftHandInterAlpha = LeftHandInterAlpha > 1 ? 1 : (LeftHandInterAlpha < 0 ? 0 : LeftHandInterAlpha);

				}

			}
			IsUsingSword = PlayerPawn->IsUsingSword;
			PickUpAlpha = PlayerPawn->PickUpAlpha;
		}
		else
		{
			AniCharacter = Cast<AMyAniCharacter>(Owner);
			if (AniCharacter)
			{
				//不是玩家，是AniCharacter
				IsAniCharacter = 1;
			}
			
		}
	}
	if (AniCharacter)
	{
		if (AniCharacter->EnableBodyIK)
		{
			if (IsAnyMontagePlaying())
			{
				//左手打人IK 射线检测

				const FVector Start = GetOwningComponent()->GetSocketLocation("lowerarm_lSocket");
				const FVector End = GetOwningComponent()->GetSocketLocation("hand_lSocket");
				const float TraceRadius = 12.0f;
				const bool IgnoreSelf = 1;
				//不要检测的actors
				TArray<AActor*> ActorsToIgnore;
				ActorsToIgnore.Add(TryGetPawnOwner());
				//检查的结果
				FHitResult HitResult;
				//检测的结果
				const bool Hit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, TraceRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility)
					, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, IgnoreSelf,
					FLinearColor::Red, FLinearColor::Green, 5.0f);

				if (Hit)
				{
					if ((LeftHandIKEffectorLocation - HitResult.Location).Size() > 10.0f)
					{
						LeftHandIKEffectorLocation = HitResult.Location;
					}
					LeftHandIKAlpha = 1;

				}
				else
				{
					LeftHandIKAlpha = LeftHandIKAlpha + DeltaTimeX * -1.0f * IKSpeed;
					LeftHandIKAlpha = LeftHandIKAlpha > 1 ? 1 : (LeftHandIKAlpha < 0 ? 0 : LeftHandIKAlpha);

				}

				
			}
			else
			{
				LeftHandIKAlpha = LeftHandIKAlpha + DeltaTimeX * -1.0f * IKSpeed;
				LeftHandIKAlpha = LeftHandIKAlpha > 1 ? 1 : (LeftHandIKAlpha < 0 ? 0 : LeftHandIKAlpha);
				

			}


			if (IsAnyMontagePlaying())
			{
				//右边手打人IK 射线检测

				const FVector Start = GetOwningComponent()->GetSocketLocation("lowerarm_rSocket");
				const FVector End = GetOwningComponent()->GetSocketLocation("hand_rSocket");
				const float TraceRadius = 12.0f;
				const bool IgnoreSelf = 1;
				//不要检测的actors
				TArray<AActor*> ActorsToIgnore;
				ActorsToIgnore.Add(TryGetPawnOwner());
				//检查的结果
				FHitResult HitResult;
				//检测的结果
				const bool Hit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, TraceRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility)
					, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, IgnoreSelf,
					FLinearColor::Red, FLinearColor::Green, 5.0f);

				if (Hit)
				{
					if ((RightHandIKEffectorLocation - HitResult.Location).Size() > 10.0f)
					{
						RightHandIKEffectorLocation = HitResult.Location;
					}
					RightHandIKAlpha = 1;
				}
				else
				{
					RightHandIKAlpha = RightHandIKAlpha + DeltaTimeX * -1.0f * IKSpeed;
					RightHandIKAlpha = RightHandIKAlpha > 1 ? 1 : (RightHandIKAlpha < 0 ? 0 : RightHandIKAlpha);

				}
			}
			else
			{
				RightHandIKAlpha = RightHandIKAlpha + DeltaTimeX * -1.0f * IKSpeed;
				RightHandIKAlpha = RightHandIKAlpha > 1 ? 1 : (RightHandIKAlpha < 0 ? 0 : RightHandIKAlpha);
			}
		}
	}
}