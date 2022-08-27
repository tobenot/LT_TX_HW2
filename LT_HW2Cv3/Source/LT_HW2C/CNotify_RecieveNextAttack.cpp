// Fill out your copyright notice in the Description page of Project Settings.


#include "CNotify_RecieveNextAttack.h"
#include "MyCharacter.h"

void UCNotify_RecieveNextAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	//连击机会窗口来了，在这之后按不同键攻击可以连击
	AMyCharacter* PlayerPawn = Cast<AMyCharacter>(MeshComp->GetOwner());
	if (PlayerPawn)
	{
		PlayerPawn->ClearContinueAttack();
		PlayerPawn->MakeSwordDamage();
	}

}
