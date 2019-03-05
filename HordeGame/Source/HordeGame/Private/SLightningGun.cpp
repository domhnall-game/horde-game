// Fill out your copyright notice in the Description page of Project Settings.

#include "SLightningGun.h"

#include "HordeGame.h"

#include "ConsoleManager.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

ASLightningGun::ASLightningGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MuzzleSocketName = "ProjectileLaunchPoint";
	ChargeUpTime = 3.f;
	AutoFireDelay = 0.05f;
	DamageMultiplierIncreaseTime = 2.f;
	DamageMultiplierDecreaseTime = 1.f;

	BaseDamage = 0.4f;
	DamageMultiplier = 1.f;
	DamageMultiplierArray.Add(1.f);
	DamageMultiplierArray.Add(2.f);
	DamageMultiplierArray.Add(4.f);
	DamageMultiplierArray.Add(10.f);
	DamageMultiplierArray.Add(25.f);
	MaxRange = 1500;

	AmmoType = EAmmoType::AMMO_Lightning;
	MaxLoadedAmmo = 1000;

	SetReplicates(true);
}

void ASLightningGun::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmo = MaxLoadedAmmo;
	CurrentDamageMultiplierIndex = 0;
	DamageMultiplier = DamageMultiplierArray[CurrentDamageMultiplierIndex];
	//UE_LOG(LogTemp, Warning, TEXT("SLightningGun -- Max clip size: %d"), MaxLoadedAmmo);
}

void ASLightningGun::Fire()
{
	if (CurrentAmmo <= 0) { return; }

	if (Role < ROLE_Authority) {
		ServerFire();
	}

	bool bHitRegistered = false;
	FHitResult Hit;
	FVector OwnerEyeLocation;
	FRotator OwnerEyeRotation;

	AActor* Owner = GetOwner();
	if (ensure(Owner)) {
		//Get the data from the owner's eyes
		Owner->GetActorEyesViewPoint(OwnerEyeLocation, OwnerEyeRotation);

		//Create the line trace end based on the vector from the owner's eyes to a very far distance
		FVector LineTraceEnd = OwnerEyeLocation + (OwnerEyeRotation.Vector() * MaxRange);

		//Set collision parameters to ignore this weapon and its owner
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Owner);
		QueryParams.AddIgnoredActor(this);

		//Trace the world from pawn eyes to crosshair location (center screen)
		bHitRegistered = GetWorld()->LineTraceSingleByChannel(Hit, OwnerEyeLocation, LineTraceEnd, COLLISION_WEAPON, QueryParams);

		//Draw a debug line for the hitscan
		extern FAutoConsoleVariableRef CVarDebugWeaponDrawing;
		if (CVarDebugWeaponDrawing->GetInt()) {
			DrawDebugLine(GetWorld(), OwnerEyeLocation, LineTraceEnd, FColor::Red, false, 1.0f, 0, 1.0f);
		}

		//If we had a hit, the end point of the tracer is the impact point of the hit; otherwise, it's wherever we set the endpoint of the trace to
		PlayFireEffects(bHitRegistered ? Hit.ImpactPoint : LineTraceEnd);

		LastFireTime = GetWorld()->TimeSeconds;
	}

	CurrentAmmo--;
	//UE_LOG(LogTemp, Warning, TEXT("SLightningGun -- Ammo left in gun: %d out of %d"), CurrentAmmo, MaxLoadedAmmo);

	//Only do damage if this is the server
	if (Role == ROLE_Authority) {
		if (bHitRegistered) {
			//The hitscan found a blocking object, so process damage
			AActor* HitActor = Hit.GetActor();

			FVector HitFromDirection = OwnerEyeRotation.Vector();
			AController* HitInstigatorController = Owner->GetInstigatorController();
			AActor* DamageCauser = this;
			UGameplayStatics::ApplyPointDamage(HitActor, BaseDamage * DamageMultiplier, HitFromDirection, Hit, HitInstigatorController, DamageCauser, DamageType);

			if (HitActor && HitActor->bCanBeDamaged) {
				StartIncreaseDamageMulitiplierTimer();
			} else {
				StartDecreaseDamageMulitiplierTimer();
			}
		} else {
			StartDecreaseDamageMulitiplierTimer();
		}
	}
}

void ASLightningGun::ServerFire_Implementation()
{
	Fire();
}

bool ASLightningGun::ServerFire_Validate()
{
	return true;
}

void ASLightningGun::StartFire()
{
	GetWorldTimerManager().SetTimer(TimerHandle_AutoFireDelay, this, &ASLightningGun::Fire, AutoFireDelay, true, ChargeUpTime);
}

void ASLightningGun::StopFire()
{
	CurrentDamageMultiplierIndex = 0;
	DamageMultiplier = DamageMultiplierArray[CurrentDamageMultiplierIndex];
	GetWorldTimerManager().ClearTimer(TimerHandle_AutoFireDelay);
	GetWorldTimerManager().ClearTimer(TimerHandle_IncreaseDamageMultiplier);
	GetWorldTimerManager().ClearTimer(TimerHandle_DecreaseDamageMultiplier);
}

int32 ASLightningGun::Reload(int32 ReloadAmount)
{
	//The lightning gun uses a charge pack, meaning that it uses the entire reload stack at once instead of individual rounds/grenades
	CurrentAmmo = ReloadAmount;
	return ReloadAmount;
}

void ASLightningGun::StartDecreaseDamageMulitiplierTimer()
{
	if (!GetWorldTimerManager().IsTimerActive(TimerHandle_DecreaseDamageMultiplier)) {
		GetWorldTimerManager().ClearTimer(TimerHandle_IncreaseDamageMultiplier);
		GetWorldTimerManager().SetTimer(TimerHandle_DecreaseDamageMultiplier, this, &ASLightningGun::DecreaseDamageMultiplier, 1.0f, false, DamageMultiplierDecreaseTime);
	}
}

void ASLightningGun::StartIncreaseDamageMulitiplierTimer()
{
	if (!GetWorldTimerManager().IsTimerActive(TimerHandle_IncreaseDamageMultiplier)) {
		GetWorldTimerManager().ClearTimer(TimerHandle_DecreaseDamageMultiplier);
		GetWorldTimerManager().SetTimer(TimerHandle_IncreaseDamageMultiplier, this, &ASLightningGun::IncreaseDamageMultiplier, 1.0f, false, DamageMultiplierIncreaseTime);
	}
}

void ASLightningGun::DecreaseDamageMultiplier()
{
	CurrentDamageMultiplierIndex = FMath::Clamp(--CurrentDamageMultiplierIndex, 0, DamageMultiplierArray.Num() - 1);
	DamageMultiplier = DamageMultiplierArray[CurrentDamageMultiplierIndex];
}

void ASLightningGun::IncreaseDamageMultiplier()
{
	CurrentDamageMultiplierIndex = FMath::Clamp(++CurrentDamageMultiplierIndex, 0, DamageMultiplierArray.Num() - 1);
	DamageMultiplier = DamageMultiplierArray[CurrentDamageMultiplierIndex];
}

void ASLightningGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASLightningGun, CurrentDamageMultiplierIndex);
}
