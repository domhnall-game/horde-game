// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"

UCLASS()
class HORDEGAME_API ASPowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTickPowerup();

	UFUNCTION()
	void OnRep_PowerupActive();

	/* Time between powerup ticks */
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	float PowerupInterval;

	/* Total times we apply the powerup effect */
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int32 TotalNumberOfTicks;

	int32 TicksProcessed;

	FTimerHandle TimerHandle_PowerupTick;

	/* Keeps the state of the powerup */
	UPROPERTY(ReplicatedUsing=OnRep_PowerupActive)
	bool bIsPowerupActive;

	UPROPERTY(BlueprintReadOnly)
	AActor* ActiveForActor;
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnActivated();
	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnExpired();
	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerupTicked();
	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerupStateChanged(bool bNewIsActive);

	void ActivatePowerup(AActor* OverlappingActor);
};
