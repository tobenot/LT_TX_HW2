// Fill out your copyright notice in the Description page of Project Settings.


#include "CNotify_EndAttack.h"
#include "MyCharacter.h"

void UCNotify_EndAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	//连击机会窗口已经过去，不允许连击
	AMyCharacter* PlayerPawn = Cast<AMyCharacter>(MeshComp->GetOwner());
	if (PlayerPawn)
	{
		PlayerPawn->ResetCombo();
	}
}
