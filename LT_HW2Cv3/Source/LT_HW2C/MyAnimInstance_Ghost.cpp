// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance_Ghost.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

/*
	敌人的动画蓝图基类
*/
void UMyAnimInstance_Ghost::CPPUpdateAnimation(float DeltaTimeX)
{
	APawn* Owner = TryGetPawnOwner();
	if (Owner)
	{
		IsInAir = Owner->GetMovementComponent()->IsFalling();
		Speed = Owner->GetVelocity().Size();
		Direction = CalculateDirection(Owner->GetVelocity(), Owner->GetBaseAimRotation());

	}
	if (!AniCharacter)
	{
		AniCharacter = Cast<AMyAniCharacter>(Owner);

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
					//IK结束时的过渡
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
