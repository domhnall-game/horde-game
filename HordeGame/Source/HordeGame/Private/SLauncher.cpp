// Fill out your copyright notice in the Description page of Project Settings.

#include "SLauncher.h"

#include "SLauncherProjectile.h"

#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"

// Sets default values
ASLauncher::ASLauncher()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MuzzleSocketName = "ProjectileLaunchPoint";
	AutoFireDelay = 1.0f;

	MaxRange = 1000;

	AmmoType = EAmmoType::AMMO_Grenade;
	MaxLoadedAmmo = 1;
}

void ASLauncher::BeginPlay()
{
	Super::BeginPlay();
	LastFireTime = -1000.f;
	CurrentAmmo = MaxLoadedAmmo;
	//UE_LOG(LogTemp, Warning, TEXT("SLauncher -- Max clip size: %d"), MaxLoadedAmmo);
}

void ASLauncher::Fire()
{
	if (CurrentAmmo <= 0) { return; }
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

	LastFireTime = GetWorld()->TimeSeconds;

	CurrentAmmo--;
	//UE_LOG(LogTemp, Warning, TEXT("SLauncher -- Ammo left in gun: %d out of %d"), CurrentAmmo, MaxLoadedAmmo);
}

void ASLauncher::StartFire()
{
	//We don't want the gun to be able to fire faster just because the player can click faster than the autofire
	float FirstDelay = FMath::Max(LastFireTime + AutoFireDelay - GetWorld()->TimeSeconds, 0.f);
	GetWorldTimerManager().SetTimer(TimerHandle_AutoFireDelay, this, &ASLauncher::Fire, AutoFireDelay, true, FirstDelay);
}

void ASLauncher::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_AutoFireDelay);
}
/*
void ASLauncher::BeginPlay()
{
	Super::BeginPlay();
}
*/
