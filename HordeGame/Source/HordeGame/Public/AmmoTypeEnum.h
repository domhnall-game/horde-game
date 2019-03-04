// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AmmoTypeEnum.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EAmmoType : uint8 {
	AMMO_Default = 0,
	AMMO_Rifle = 1,
	AMMO_Grenade = 2,
	AMMO_Lightning = 3,
	AMMO_Max
};
