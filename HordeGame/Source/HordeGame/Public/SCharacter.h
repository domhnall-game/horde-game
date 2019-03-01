// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AmmoTypeEnum.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASWeapon;

UCLASS()
class HORDEGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void BeginCrouch();
	void EndCrouch();

	void StartAim();
	void EndAim();
	void Aim(float DeltaTime);
	void StartFire();
	void StopFire();
	void Reload();
	void SwitchToRifle();
	void SwitchToLauncher();
	void SwitchToLightningGun();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	bool bIsAiming = false;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClamMin = 0.1f, ClampMax = 100.f))
	float AimInterpSpeed = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float AimDownSightsFOV = 60.f;
	float DefaultFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TArray<TSubclassOf<ASWeapon>> WeaponList;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	ASWeapon* CurrentWeapon;

	TArray<ASWeapon*> EquippedWeapons;

	UPROPERTY(VisibleDefaultsOnly, Category = "Ammo")
	TMap<EAmmoType, int32> MaxAmmoPerType;
	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 MaxRifleAmmo;
	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 MaxGrenades;
	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 MaxLightningCharge;

	TMap<EAmmoType, int32> CurrentAmmoPerType;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName = "weapon_socket";
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Override of Pawn's GetPawnViewLocation function; base version uses pawn eyes, but we want to use the camera
	virtual FVector GetPawnViewLocation() const override;
	
	void AddAmmo(EAmmoType AmmoType, int32 AmmoAmount);

	bool bIsReloading;
};
