// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AmmoTypeEnum.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SAmmoInventory.generated.h"

//I got very, very fucking tired of UE4's bullshit
//So this Actor class
//That will NEVER BE FUCKING CREATED EVER
//Just exist so I can have some STATIC FUCKING METHODS TO INITIALIZE SHIT
//What the FUCK UE4
UCLASS()
class HORDEGAME_API ASAmmoInventory : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASAmmoInventory();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	static TMap<EAmmoType, int32> InitializedMaxAmmoMap();
	static TMap<EAmmoType, int32> InitializedDefaultAmmoMap();
	static TMap<EAmmoType, int32> InitializedPickupAmmoMap();

	/*****************************************************************
	 * Max, Default, and Clip Ammo Constants
	 *****************************************************************/
private:
	static const int32 DEFAULT_MAX_AMMO_RIFLE = 600;
	static const int32 DEFAULT_MAX_AMMO_GRENADE = 20;
	static const int32 DEFAULT_MAX_AMMO_LIGHTNING = 1000;
	static const int32 DEFAULT_STARTING_AMMO_RIFLE = 60;
	static const int32 DEFAULT_STARTING_AMMO_GRENADE = 5;
	static const int32 DEFAULT_STARTING_AMMO_LIGHTNING = 1000;
	static const int32 DEFAULT_PICKUP_SIZE_RIFLE = 30;
	static const int32 DEFAULT_PICKUP_SIZE_GRENADE = 5;
	static const int32 DEFAULT_PICKUP_SIZE_LIGHTNING = 1000;
};
