// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SLauncher.generated.h"

class ASLauncherProjectile;

UCLASS()
class HORDEGAME_API ASLauncher : public ASWeapon
{
	GENERATED_BODY()
	
public:
	ASLauncher();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Fire() override;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<ASLauncherProjectile> ProjectileClass;
};
