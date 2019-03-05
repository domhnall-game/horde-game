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
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerFire() override;

	void StartDecreaseDamageMulitiplierTimer();
	void StartIncreaseDamageMulitiplierTimer();

	void DecreaseDamageMultiplier();
	void IncreaseDamageMultiplier();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float ChargeUpTime;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float DamageMultiplierIncreaseTime;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float DamageMultiplierDecreaseTime;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TArray<float> DamageMultiplierArray;
	UPROPERTY(Replicated)
	int32 CurrentDamageMultiplierIndex;

	FTimerHandle TimerHandle_ChargeUp;
	FTimerHandle TimerHandle_DecreaseDamageMultiplier;
	FTimerHandle TimerHandle_IncreaseDamageMultiplier;

public:
	virtual void StartFire() override;
	virtual void StopFire() override;

	virtual int32 Reload(int32 ReloadAmount) override;
};
