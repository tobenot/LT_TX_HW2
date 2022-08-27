// Fill out your copyright notice in the Description page of Project Settings.


#include "CNotify_NextAttack.h"
#include "MyCharacter.h"

void UCNotify_NextAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	//检测是否要继续连击
	AMyCharacter* PlayerPawn = Cast<AMyCharacter>(MeshComp->GetOwner());
	if (PlayerPawn)
	{
		PlayerPawn->ContinueAttackEvent();
	}
}
