// Fill out your copyright notice in the Description page of Project Settings.

#include "STrackerBot.h"

#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "AI/Navigation/NavigationPath.h"
#include "AI/Navigation/NavigationSystem.h"
#include "SHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCanEverAffectNavigation(false);
	MeshComponent->SetSimulatePhysics(true);
	RootComponent = MeshComponent;

	HealthComponent = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnHealthChanged.AddDynamic(this, &ASTrackerBot::OnHealthChanged);

	bUseVelocityChange = true;
	MovementForce = 500;
	RequiredDistanceToTarget = 100;

	BaseDamage = 50;
	ExplosionRadiusInner = 100;
	ExplosionRadiusOuter = 200;
	DamageFalloff = 0.5f;
	bExploded = false;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	//Get the dynamic material instance
	DynamicMaterialInst = MeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComponent->GetMaterial(0));
	
	NextPathPoint = GetNextPathPoint();
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

	if (DistanceToTarget <= RequiredDistanceToTarget) {
		NextPathPoint = GetNextPathPoint();
	} else {
		//Keep moving towards target
		FVector ForceDirection = NextPathPoint - GetActorLocation();
		ForceDirection.Normalize();
		ForceDirection *= MovementForce;

		MeshComponent->AddForce(ForceDirection, NAME_None, bUseVelocityChange);
	}
}

FVector ASTrackerBot::GetNextPathPoint()
{
	//Hack to get player location
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

	UNavigationPath* NavPath = UNavigationSystem::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);
	if (NavPath->PathPoints.Num() > 1) {
		//Return next point in path
		return NavPath->PathPoints[1];
	}

	//Failed to find a path
	return GetActorLocation();
}

void ASTrackerBot::SelfDestruct()
{
	if (bExploded) {
		return;
	}

	bExploded = true;

	//Play explosion particle
	if (ExplosionEffect) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	}

	//Apply damage
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);
	UGameplayStatics::ApplyRadialDamageWithFalloff(this, BaseDamage, 20, GetActorLocation(), ExplosionRadiusInner, ExplosionRadiusOuter, DamageFalloff, nullptr, IgnoredActors, this, GetInstigatorController());
	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadiusInner, 12, FColor::Red, false, 2.f, 0, 3.f);
	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadiusOuter, 12, FColor::Yellow, false, 2.f, 0, 3.f);

	//Get rid of actor
	Destroy();
}

void ASTrackerBot::OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("%s: %f HP"), *GetName(), Health);

	//Pulse material on hit
	if (DynamicMaterialInst) {
		DynamicMaterialInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	//Explode on death
	if (Health <= 0.f) {
		SelfDestruct();
	}
}
