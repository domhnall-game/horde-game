// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"

#include "HordeGame.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVarDebugWeaponDrawing(TEXT("HORDE.DebugWeapons"), DebugWeaponDrawing, TEXT("Draw debug lines for weapon hits"), ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "BeamEnd";

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	BaseDamage = 20.f;
	HeadshotMultiplier = 1.f;
	AutoFireDelay = 0.2f;

	AmmoType = EAmmoType::AMMO_Rifle;
	MaxLoadedAmmo = 30;
}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	LastFireTime = -1000.f;
	CurrentAmmo = MaxLoadedAmmo;
}

void ASWeapon::Fire()
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
		//When set to true, it provides details based on exact part of the mesh hit, not just the collision box
		QueryParams.bTraceComplex = true;
		//If this is not set to true, we do not get the physical material of the part of the mesh that was hit
		QueryParams.bReturnPhysicalMaterial = true;

		//Trace the world from pawn eyes to crosshair location (center screen)
		FHitResult Hit;
		bool bHitRegistered = GetWorld()->LineTraceSingleByChannel(Hit, OwnerEyeLocation, LineTraceEnd, COLLISION_WEAPON, QueryParams);

		if (bHitRegistered) {
			//The hitscan found a blocking object, so process damage
			AActor* HitActor = Hit.GetActor();

			//Get the physical material of the part of the surface that was hit
			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			//Determine appropriate particle effect and damage from the physical material
			UParticleSystem* SelectedEffect = nullptr;
			float Damage = BaseDamage;
			switch (SurfaceType) {
			case SURFACE_FLESH_VULN:
				Damage *= HeadshotMultiplier;
			case SURFACE_FLESH_DEFAULT:
				SelectedEffect = FleshImpactEffect;
				break;
			default:
				SelectedEffect = DefaultImpactEffect;
				break;
			}

			FVector HitFromDirection = OwnerEyeRotation.Vector();
			AController* HitInstigatorController = Owner->GetInstigatorController();
			AActor* DamageCauser = this;
			UGameplayStatics::ApplyPointDamage(HitActor, Damage, HitFromDirection, Hit, HitInstigatorController, DamageCauser, DamageType);


			if (ensure(SelectedEffect)) {
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}
		}

		//Draw a debug line for the hitscan
		if (DebugWeaponDrawing > 0) {
			DrawDebugLine(GetWorld(), OwnerEyeLocation, LineTraceEnd, FColor::Red, false, 1.0f, 0, 1.0f);
		}

		//If we had a hit, the end point of the tracer is the impact point of the hit; otherwise, it's wherever we set the endpoint of the trace to
		PlayFireEffects(bHitRegistered ? Hit.ImpactPoint : LineTraceEnd);

		LastFireTime = GetWorld()->TimeSeconds;
	}

	CurrentAmmo--;
}

void ASWeapon::StartFire()
{
	//We don't want the gun to be able to fire faster just because the player can click faster than the autofire
	float FirstDelay = FMath::Max(LastFireTime + AutoFireDelay - GetWorld()->TimeSeconds, 0.f);
	GetWorldTimerManager().SetTimer(TimerHandle_AutoFireDelay, this, &ASWeapon::Fire, AutoFireDelay, true, FirstDelay);
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_AutoFireDelay);
}

int32 ASWeapon::Reload(int32 ReloadAmount) {
	int32 AmmoToReload = MaxLoadedAmmo - CurrentAmmo;

	if (ReloadAmount < AmmoToReload) {
		CurrentAmmo += ReloadAmount;
		return ReloadAmount;
	} else {
		CurrentAmmo += AmmoToReload;
		return AmmoToReload;
	}
}

void ASWeapon::PlayFireEffects(FVector ParticleEndVector)
{
	//Play the muzzle flash
	if (ensure(MuzzleEffect)) {
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComponent, MuzzleSocketName);
	}

	if (ensure(TracerEffect)) {
		FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);
		UParticleSystemComponent* TracerParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerParticle) {
			//Target name comes from the tracer particle itself, under Target->ParameterName
			TracerParticle->SetVectorParameter(TracerTargetName, ParticleEndVector);
		}
	}

	if (FireCamShake) {
		//Camera shake
		APawn* Owner = Cast<APawn>(GetOwner());
		if (Owner) {
			APlayerController* PlayerController = Cast<APlayerController>(Owner->GetController());
			if (PlayerController) {
				PlayerController->ClientPlayCameraShake(FireCamShake);
			}
		}
	}
}

// Called when the game starts or when spawned
/*
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}
*/

// Called every frame
/*
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
*/
