// Fill out your copyright notice in the Description page of Project Settings.

#include "SLightningGun.h"

#include "HordeGame.h"

#include "ConsoleManager.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"

ASLightningGun::ASLightningGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MuzzleSocketName = "ProjectileLaunchPoint";
	ChargeUpTime = 3.0f;
	AutoFireDelay = 0.05f;
	BaseDamage = 1.0f;
	HeadshotMultiplier = 1.0f;

	AmmoType = EAmmoType::AMMO_Lightning;
	MaxLoadedAmmo = 1000;
}

void ASLightningGun::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmo = MaxLoadedAmmo;
}

void ASLightningGun::Fire()
{
	if (CurrentAmmo <= 0) { return; }

	AActor* Owner = GetOwner();

	if (ensure(Owner)) {
		//Get the data from the owner's eyes
		FVector OwnerEyeLocation;
		FRotator OwnerEyeRotation;
		Owner->GetActorEyesViewPoint(OwnerEyeLocation, OwnerEyeRotation);

		//Create the line trace end based on the vector from the owner's eyes to a very far distance
		FVector LineTraceEnd = OwnerEyeLocation + (OwnerEyeRotation.Vector() * 10000);

		//Set collision parameters to ignore this weapon and its owner
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Owner);
		QueryParams.AddIgnoredActor(this);

		//Trace the world from pawn eyes to crosshair location (center screen)
		FHitResult Hit;
		bool bHitRegistered = GetWorld()->LineTraceSingleByChannel(Hit, OwnerEyeLocation, LineTraceEnd, COLLISION_WEAPON, QueryParams);

		if (bHitRegistered) {
			//The hitscan found a blocking object, so process damage
			AActor* HitActor = Hit.GetActor();

			FVector HitFromDirection = OwnerEyeRotation.Vector();
			AController* HitInstigatorController = Owner->GetInstigatorController();
			AActor* DamageCauser = this;
			UGameplayStatics::ApplyPointDamage(HitActor, BaseDamage, HitFromDirection, Hit, HitInstigatorController, DamageCauser, DamageType);
		}

		//Draw a debug line for the hitscan
		extern FAutoConsoleVariableRef CVarDebugWeaponDrawing;
		if (CVarDebugWeaponDrawing->GetInt()) {
			DrawDebugLine(GetWorld(), OwnerEyeLocation, LineTraceEnd, FColor::Red, false, 1.0f, 0, 1.0f);
		}

		//If we had a hit, the end point of the tracer is the impact point of the hit; otherwise, it's wherever we set the endpoint of the trace to
		PlayFireEffects(bHitRegistered ? Hit.ImpactPoint : LineTraceEnd);

		//FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);
		//DrawDebugLine(GetWorld(), MuzzleLocation, LineTraceEnd, FColor::Blue, false, -1.f, 0, 5.0f);

		LastFireTime = GetWorld()->TimeSeconds;
	}

	CurrentAmmo--;
}

void ASLightningGun::StartFire()
{
	GetWorldTimerManager().SetTimer(TimerHandle_AutoFireDelay, this, &ASLightningGun::Fire, AutoFireDelay, true, ChargeUpTime);
}

void ASLightningGun::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_AutoFireDelay);
}

int32 ASLightningGun::Reload(int32 ReloadAmount)
{
	//The lightning gun uses a charge pack, meaning that it uses the entire reload stack at once instead of individual rounds/grenades
	CurrentAmmo = ReloadAmount;
	return ReloadAmount;
}
