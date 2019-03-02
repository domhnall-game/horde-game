// Fill out your copyright notice in the Description page of Project Settings.

#include "SExplodingBarrel.h"

#include "SHealthComponent.h"

#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ASExplodingBarrel::ASExplodingBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_Destructible);
	RootComponent = MeshComponent;

	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComponent"));
	RadialForceComponent->SetupAttachment(RootComponent);
	RadialForceComponent->bAutoActivate = false;

	HealthComponent = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));

	bIsExploded = false;

	ExplosionDamage = 200;
}

// Called when the game starts or when spawned
void ASExplodingBarrel::BeginPlay()
{
	Super::BeginPlay();

	HealthComponent->OnHealthChanged.AddDynamic(this, &ASExplodingBarrel::OnHealthChanged);
}

void ASExplodingBarrel::OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* InputDamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	//Death
	if (Health <= 0.0f && !bIsExploded) {
		bIsExploded = true;

		if (ExplosionEffect) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation(), GetActorRotation());
		}

		MeshComponent->AddImpulse(GetActorLocation().UpVector * RadialForceComponent->ImpulseStrength);

		MeshComponent->SetMaterial(0, Cast<UMaterialInterface>(ExplodedMaterial));

		RadialForceComponent->FireImpulse();

		TArray<AActor*> IgnoreActors;
		UGameplayStatics::ApplyRadialDamageWithFalloff(this, ExplosionDamage, 0, GetActorLocation(), RadialForceComponent->Radius / 2, RadialForceComponent->Radius, 0.5f, DamageType, IgnoreActors, this, InstigatedBy);
	}
}

// Called every frame
/*
void ASExplodingBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
*/
