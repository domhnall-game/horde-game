// Fill out your copyright notice in the Description page of Project Settings.

#include "SAmmoInventory.h"

// Sets default values
ASAmmoInventory::ASAmmoInventory()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ASAmmoInventory::BeginPlay()
{
	Super::BeginPlay();
	
}

TMap<EAmmoType, int32> ASAmmoInventory::InitializedMaxAmmoMap()
{
	TMap<EAmmoType, int32> InitializedMaxAmmoMap;
	InitializedMaxAmmoMap.Add(EAmmoType::AMMO_Default, 0);
	InitializedMaxAmmoMap.Add(EAmmoType::AMMO_Rifle, DEFAULT_MAX_AMMO_RIFLE);
	InitializedMaxAmmoMap.Add(EAmmoType::AMMO_Grenade, DEFAULT_MAX_AMMO_GRENADE);
	InitializedMaxAmmoMap.Add(EAmmoType::AMMO_Lightning, DEFAULT_MAX_AMMO_LIGHTNING);
	InitializedMaxAmmoMap.Add(EAmmoType::AMMO_Max, 0);
	return InitializedMaxAmmoMap;
}

TMap<EAmmoType, int32> ASAmmoInventory::InitializedDefaultAmmoMap()
{
	TMap<EAmmoType, int32> InitializedDefaultAmmoMap;
	InitializedDefaultAmmoMap.Add(EAmmoType::AMMO_Default, 0);
	InitializedDefaultAmmoMap.Add(EAmmoType::AMMO_Rifle, DEFAULT_STARTING_AMMO_RIFLE);
	InitializedDefaultAmmoMap.Add(EAmmoType::AMMO_Grenade, DEFAULT_STARTING_AMMO_GRENADE);
	InitializedDefaultAmmoMap.Add(EAmmoType::AMMO_Lightning, DEFAULT_STARTING_AMMO_LIGHTNING);
	InitializedDefaultAmmoMap.Add(EAmmoType::AMMO_Max, 0);
	return InitializedDefaultAmmoMap;
}

TMap<EAmmoType, int32> ASAmmoInventory::InitializedPickupAmmoMap()
{
	TMap<EAmmoType, int32> InitializedPickupAmmoMap;
	InitializedPickupAmmoMap.Add(EAmmoType::AMMO_Default, 0);
	InitializedPickupAmmoMap.Add(EAmmoType::AMMO_Rifle, DEFAULT_PICKUP_SIZE_RIFLE);
	InitializedPickupAmmoMap.Add(EAmmoType::AMMO_Grenade, DEFAULT_PICKUP_SIZE_GRENADE);
	InitializedPickupAmmoMap.Add(EAmmoType::AMMO_Lightning, DEFAULT_PICKUP_SIZE_LIGHTNING);
	InitializedPickupAmmoMap.Add(EAmmoType::AMMO_Max, 0);
	return InitializedPickupAmmoMap;
}
