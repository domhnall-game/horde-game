// Fill out your copyright notice in the Description page of Project Settings.

#include "SAmmoPickup.h"

#include "SCharacter.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASAmmoPickup::ASAmmoPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Use a sphere as a simple collision representation
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->InitSphereRadius(5.0f);
	CollisionComponent->SetCollisionProfileName("Projectile");
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	// Players can't walk on it
	CollisionComponent->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComponent->CanCharacterStepUpOn = ECB_No;
	RootComponent = CollisionComponent;

	//Set up a mesh component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetupAttachment(RootComponent);

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASAmmoPickup::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority) {
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASAmmoPickup::OnOverlapBegin);	// set up a notification for when this component hits something overlapping
	}
}

// Called every frame
void ASAmmoPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASAmmoPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor) {
		ASCharacter* Character = Cast<ASCharacter>(OtherActor);
		if (Character) {
			/*
			if (AmmoType == EAmmoType::AMMO_Grenade) {
				UE_LOG(LogTemp, Warning, TEXT("SAmmoPickup -- Received %d ammo for ammo type grenade"), PickupAmount);
			} else if (AmmoType == EAmmoType::AMMO_Rifle) {
				UE_LOG(LogTemp, Warning, TEXT("SAmmoPickup -- Received %d ammo for ammo type rifle"), PickupAmount);
			} else if (AmmoType == EAmmoType::AMMO_Lightning) {
				UE_LOG(LogTemp, Warning, TEXT("SAmmoPickup -- Received %d ammo for ammo type lightning"), PickupAmount);
			}
			*/
			Character->AddAmmo(AmmoType, PickupAmount);
			Destroy();
		}
	}
}
