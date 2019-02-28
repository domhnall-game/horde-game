// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AmmoTypeEnum.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SAmmoPickup.generated.h"

class USphereComponent;

UCLASS()
class HORDEGAME_API ASAmmoPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASAmmoPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	USphereComponent* CollisionComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	EAmmoType AmmoType;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 PickupAmount;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	EAmmoType GetAmmoType() { return AmmoType; }
	int32 GetPickupAmount() { return PickupAmount; }
	
};
