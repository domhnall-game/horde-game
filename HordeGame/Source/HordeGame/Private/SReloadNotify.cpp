// Fill out your copyright notice in the Description page of Project Settings.

#include "SReloadNotify.h"

#include "SCharacter.h"

#include "Components/SkeletalMeshComponent.h"

void USReloadNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AActor* AnimationOwner = MeshComp->GetOwner();
	if (ASCharacter* Character = Cast<ASCharacter>(AnimationOwner)) {
		Character->bIsReloading = false;
	}
}


