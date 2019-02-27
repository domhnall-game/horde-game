// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Engine/World.h"


// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MuzzleSocketName = "MuzzleSocket";

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
}

void ASWeapon::Fire()
{
	//Play the muzzle flash
	if (ensure(MuzzleEffect)) {
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComponent, MuzzleSocketName);
	}

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

		//Trace the world from pawn eyes to crosshair location (center screen)
		FHitResult Hit;

		if (GetWorld()->LineTraceSingleByChannel(Hit, OwnerEyeLocation, LineTraceEnd, ECC_Visibility, QueryParams)) {
			//The hitscan found a blocking object, so process damage
			AActor* HitActor = Hit.GetActor();

			float Damage = 20.0f;
			FVector HitFromDirection = OwnerEyeRotation.Vector();
			AController* HitInstigatorController = Owner->GetInstigatorController();
			AActor* DamageCauser = this;
			UGameplayStatics::ApplyPointDamage(HitActor, Damage, HitFromDirection, Hit, HitInstigatorController, DamageCauser, DamageType);

			if (ensure(ImpactEffect)) {
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}
		}

		//Draw a debug line for the hitscan
		DrawDebugLine(GetWorld(), OwnerEyeLocation, LineTraceEnd, FColor::Red, false, 1.0f, 0, 1.0f);
	}
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

