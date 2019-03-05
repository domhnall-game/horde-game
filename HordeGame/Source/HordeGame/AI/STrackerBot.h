// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

class USHealthComponent;

UCLASS()
class HORDEGAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector GetNextPathPoint();
	void SelfDestruct();

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USHealthComponent* HealthComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	//Dynamic material to pulse on damage
	UMaterialInstanceDynamic* DynamicMaterialInst;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* ExplosionEffect;

	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float ExplosionRadiusOuter;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float ExplosionRadiusInner;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageFalloff;

	UPROPERTY(EditDefaultsOnly, Category = "Tracking")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "Tracking")
	float RequiredDistanceToTarget;

	UPROPERTY(EditDefaultsOnly, Category = "Tracking")
	bool bUseVelocityChange;

	bool bExploded;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;	
};
