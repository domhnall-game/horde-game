// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPickupActor.generated.h"

class ASPowerupActor;
class USphereComponent;

UCLASS()
class HORDEGAME_API ASPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPickupActor();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Respawn();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComponent;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UDecalComponent* DecalComponent;

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
	TSubclassOf<ASPowerupActor> PowerupClass;

	ASPowerupActor* PowerupInstance;

	UPROPERTY(EditDefaultsOnly, Category = "PickupActor")
	float CooldownDuration;

	FTimerHandle TimerHandle_RespawnTimer;
};
