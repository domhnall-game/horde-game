// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SLightningGun.generated.h"

UCLASS()
class HORDEGAME_API ASLightningGun : public ASWeapon
{
	GENERATED_BODY()
	

public:
	ASLightningGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Fire() override;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float ChargeUpTime;

	FTimerHandle TimerHandle_ChargeUp;

public:
	virtual void StartFire() override;
	virtual void StopFire() override;
	virtual int32 Reload(int32 ReloadAmount) override;
};