// Fill out your copyright notice in the Description page of Project Settings.

#include "SReloadNotify.h"

#include "SCharacter.h"

#include "Components/SkeletalMeshComponent.h"

void USReloadNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AActor* AnimationOwner = MeshComp->GetOwner();
	if (ASCharacter* Character = Cast<ASCharacter>(AnimationOwner)) {
		UE_LOG(LogTemp, Warning, TEXT("USReloadNotify -- Custom animation notification ReloadNotify fired; setting %s bIsReloading to false"), *Character->GetName());
		Character->bIsReloading = false;
	}
}


