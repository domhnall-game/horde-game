// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "SLauncherProjectile.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

ASLauncherProjectile::ASLauncherProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->InitSphereRadius(5.0f);
	CollisionComponent->SetCollisionProfileName("Projectile");

	// Players can't walk on it
	CollisionComponent->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComponent->CanCharacterStepUpOn = ECB_No;
	RootComponent = CollisionComponent;

	//Set up a mesh component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	ProjectileMovement->UpdatedComponent = RootComponent;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	bIsExploded = false;

	SetReplicates(true);
}

void ASLauncherProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority) {
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASLauncherProjectile::OnOverlapBegin);	// set up a notification for when this component hits something blocking
		GetWorldTimerManager().SetTimer(TimerHandle_ExplosionDelay, this, &ASLauncherProjectile::Explode, 1.0f);
	}
}

void ASLauncherProjectile::Explode()
{
	if (!bIsExploded) {
		bIsExploded = true;
		OnRep_IsExploded();
		//LauncherProjectiles are ONLY ever spawned by the server (see SLauncher.Fire())
		//So there's no need for a ServerExplode() function, we only need an authority check for the damage
		if (Role == ROLE_Authority) {
			TArray<AActor*> IgnoreActors;
			UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), 50.f, 10.f, GetActorLocation(), 50.f, 500.f, 1.0f, DamageType, IgnoreActors, this, GetInstigatorController());
			SetLifeSpan(1.f);
			MeshComponent->SetVisibility(false);
			CollisionComponent->Deactivate();
			ProjectileMovement->Deactivate();
			//Destroy();
		}
	}
}

void ASLauncherProjectile::OnRep_IsExploded()
{
	//DrawDebugSphere(GetWorld(), GetActorLocation(), 200.f, 12, FColor::Red, false, 1.0f, 0, 1.0f);
	if (ensure(ExplosionEffect)) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation(), GetActorRotation());
	}
}

void ASLauncherProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this) { return; }

	Explode();
}

void ASLauncherProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASLauncherProjectile, bIsExploded);
}

/*
void ASLauncherProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == this) { return; }

	Explode();
}
*/
