// Fill out your copyright notice in the Description page of Project Settings.

#include "STrackerBot.h"

#include "SCharacter.h"

#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "AI/Navigation/NavigationPath.h"
#include "AI/Navigation/NavigationSystem.h"
#include "SHealthComponent.h"
#include "Sound/SoundCue.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"

// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCanEverAffectNavigation(false);
	MeshComponent->SetSimulatePhysics(true);
	RootComponent = MeshComponent;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComponent->SetupAttachment(RootComponent);

	HealthComponent = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));

	bUseVelocityChange = true;
	MovementForce = 500.f;
	RequiredDistanceToTarget = 100.f;

	RollingVolumeMinSpeed = 10.f;
	RollingVolumeMaxSpeed = 1000.f;
	RollingVolumeMinLoudness = 0.1f;
	RollingVolumeMaxLoudness = 2.f;

	BaseDamage = 50.f;
	ExplosionRadiusInner = 100.f;
	ExplosionRadiusOuter = 200.f;
	DamageFalloff = 0.5f;
	SelfDamageInterval = 0.5f;
	bExploded = false;

	AIProcessingInterval = 0.1f;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	//Get the dynamic material instance
	DynamicMaterialInst = MeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComponent->GetMaterial(0));
	if (DynamicMaterialInst) {
		DynamicMaterialInst->SetScalarParameterValue("DefaultHealth", HealthComponent->GetDefaultHealth());
		DynamicMaterialInst->SetScalarParameterValue("CurrentHealth", HealthComponent->GetCurrentHealth());
	}

	HealthComponent->OnHealthChanged.AddDynamic(this, &ASTrackerBot::OnHealthChanged);
	SphereComponent->SetSphereRadius(ExplosionRadiusOuter);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ASTrackerBot::OnOverlapBegin);

	RollingVolumeInputSpeed = FVector2D(RollingVolumeMinSpeed, RollingVolumeMaxSpeed);
	RollingVolumeOutputLoudness = FVector2D(RollingVolumeMinLoudness, RollingVolumeMaxLoudness);
	
	NextPathPoint = GetNextPathPoint();

	UGameplayStatics::SpawnSoundAttached(RollingSound, RootComponent);

	GetWorldTimerManager().SetTimer(TimerHandle_ProcessAI, this, &ASTrackerBot::ProcessAI, AIProcessingInterval, true, 0.0f);
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());

	//Get rid of actor
	Destroy();
}

void ASTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

void ASTrackerBot::OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("%s: %f HP"), *GetName(), Health);

	//Pulse material on hit
	if (DynamicMaterialInst) {
		DynamicMaterialInst->SetScalarParameterValue("CurrentHealth", Health);
	}

	//Explode on death
	if (Health <= 0.f) {
		SelfDestruct();
	}
}

void ASTrackerBot::ProcessAI()
{
	//float DistanceToTarget = (GetActorLocation() - GetNextPathPoint()).Size();

	//if (DistanceToTarget <= RequiredDistanceToTarget) {
	//	NextPathPoint = GetNextPathPoint();
	//} else {
		//Keep moving towards target
		FVector ForceDirection = GetNextPathPoint() - GetActorLocation();
		ForceDirection.Normalize();
		ForceDirection *= (MovementForce / GetWorld()->GetDeltaSeconds()*AIProcessingInterval);

		MeshComponent->AddForce(ForceDirection, NAME_None, bUseVelocityChange);
	//}

	if (RollingSound) {
		FVector RollingVelocity = GetVelocity();
		FVector RollingVelocityDirection;
		float RollingVelocityLength;
		RollingVelocity.ToDirectionAndLength(RollingVelocityDirection, RollingVelocityLength);

		RollingSound->VolumeMultiplier = FMath::GetMappedRangeValueClamped(RollingVolumeInputSpeed, RollingVolumeOutputLoudness, RollingVelocityLength);
	}
}

void ASTrackerBot::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bStartedSelfDestruct) {
		ASCharacter* PlayerPawn = Cast<ASCharacter>(OtherActor);
		if (PlayerPawn) {
			//Start self-destruction sequence
			GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASTrackerBot::DamageSelf, SelfDamageInterval, true, 0.0f);
			bStartedSelfDestruct = true;
			UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent);
		}
	}
}
