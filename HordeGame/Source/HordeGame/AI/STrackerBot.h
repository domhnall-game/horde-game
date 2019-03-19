// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

class ASCharacter;
class USHealthComponent;
class USoundCue;
class USphereComponent;

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
	void ChooseTarget();

	UFUNCTION()
	void DamageSelf();

	UFUNCTION()
	void ProcessAI();

	UFUNCTION()
	void OnRep_StartedSelfDestruct();

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USHealthComponent* HealthComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* SphereComponent;

	//Dynamic material to pulse on damage
	UMaterialInstanceDynamic* DynamicMaterialInst;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float SelfDamageInterval;

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

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	float RollingVolumeMinSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	float RollingVolumeMaxSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	float RollingVolumeMinLoudness;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	float RollingVolumeMaxLoudness;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundCue* RollingSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundCue* SelfDestructSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundCue* ExplodeSound;

	ASCharacter* Target;

	float AIProcessingInterval;

	UPROPERTY(ReplicatedUsing=OnRep_StartedSelfDestruct)
	bool bStartedSelfDestruct;
	bool bExploded;
	FTimerHandle TimerHandle_SelfDamage;
	FTimerHandle TimerHandle_ProcessAI;
	FVector2D RollingVolumeInputSpeed;
	FVector2D RollingVolumeOutputLoudness;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;	
};
