// Fill out your copyright notice in the Description page of Project Settings.

#include "SPowerupActor.h"

#include "TimerManager.h"

// Sets default values
ASPowerupActor::ASPowerupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	PowerupInterval = 0.f;
	TotalNumberOfTicks = 0;
}

// Called when the game starts or when spawned
void ASPowerupActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASPowerupActor::OnTickPowerup()
{
	OnPowerupTicked();

	if (TicksProcessed > TotalNumberOfTicks) {
		OnExpired();
		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}

	TicksProcessed++;
}

void ASPowerupActor::ActivatePowerup()
{
	if (PowerupInterval > 0.f) {
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASPowerupActor::OnTickPowerup, PowerupInterval, true, 0.f);
	} else {
		OnTickPowerup();
	}
}
