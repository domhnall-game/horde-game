// Fill out your copyright notice in the Description page of Project Settings.

#include "SLauncher.h"

#include "SLauncherProjectile.h"

#include "Components/SkeletalMeshComponent.h"

// Sets default values
ASLauncher::ASLauncher()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MuzzleSocketName = "ProjectileLaunchPoint";
}

void ASLauncher::Fire()
{
	AActor* Owner = GetOwner();
	// try and fire a projectile
	if (ensure(ProjectileClass) && ensure(Owner))
	{
		//We want to fire from the muzzle location, but at an angle based on the camera, not the muzzle
		FVector OwnerEyeLocation;
		FRotator OwnerEyeRotation;
		Owner->GetActorEyesViewPoint(OwnerEyeLocation, OwnerEyeRotation);
		FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		ActorSpawnParams.Owner = this;
		ActorSpawnParams.Instigator = Cast<APawn>(Owner);

		// spawn the projectile at the muzzle
		ASLauncherProjectile* Projectile = GetWorld()->SpawnActor<ASLauncherProjectile>(ProjectileClass, MuzzleLocation, OwnerEyeRotation, ActorSpawnParams);
	}
}
/*
void ASLauncher::BeginPlay()
{
	Super::BeginPlay();
}
*/
