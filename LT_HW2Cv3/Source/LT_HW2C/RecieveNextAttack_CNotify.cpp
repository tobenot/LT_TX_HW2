// Fill out your copyright notice in the Description page of Project Settings.


#include "RecieveNextAttack_CNotify.h"
#include "MyCharacter.h"

/*

	���û���ã�
*/
void URecieveNextAttack_CNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	
	AMyCharacter* PlayerPawn = Cast<AMyCharacter>(MeshComp->GetOwner());
	if (PlayerPawn)
	{
		PlayerPawn->ContinueAttackEvent();
	}
	
}
