// Fill out your copyright notice in the Description page of Project Settings.


#include "CNotify_RecieveNextAttack.h"
#include "MyCharacter.h"

void UCNotify_RecieveNextAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	//�������ᴰ�����ˣ�����֮�󰴲�ͬ��������������
	AMyCharacter* PlayerPawn = Cast<AMyCharacter>(MeshComp->GetOwner());
	if (PlayerPawn)
	{
		PlayerPawn->ClearContinueAttack();
		PlayerPawn->MakeSwordDamage();
	}

}
