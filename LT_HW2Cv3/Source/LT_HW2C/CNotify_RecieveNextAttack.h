// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CNotify_RecieveNextAttack.generated.h"

/**
 * 
 */
UCLASS()
class LT_HW2C_API UCNotify_RecieveNextAttack : public UAnimNotify
{
	GENERATED_BODY()
	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
