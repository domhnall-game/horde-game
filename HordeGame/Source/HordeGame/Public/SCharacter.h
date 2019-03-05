// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AmmoTypeEnum.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class ASWeapon;

class UCameraComponent;
class USHealthComponent;
class USpringArmComponent;

USTRUCT()
struct FAmmoInventory
{
	GENERATED_BODY();

public:
	UPROPERTY()
	EAmmoType AmmoType;

	UPROPERTY()
	int32 AmmoAmount;
};

UCLASS()
class HORDEGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

	/******************************************************************************************
	 * Auto generated functions
	 *****************************************************************************************/
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Sets default values for this character's properties
	ASCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	/******************************************************************************************
	  * Health and death
	  *****************************************************************************************/
protected:
	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bIsDead;

	/******************************************************************************************
	  * Movement
	  *****************************************************************************************/
protected:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void BeginCrouch();
	void EndCrouch();

	/******************************************************************************************
	  * Aiming
	  *****************************************************************************************/
protected:
	void StartAim();
	void EndAim();
	void Aim(float DeltaTime);

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClamMin = 0.1f, ClampMax = 100.f))
	float AimInterpSpeed = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float AimDownSightsFOV = 60.f;
	float DefaultFOV;

	bool bIsAiming = false;
public:
	//Override of Pawn's GetPawnViewLocation function; base version uses pawn eyes, but we want to use the camera
	virtual FVector GetPawnViewLocation() const override;

	/******************************************************************************************
	  * Firing and reloading weapons
	  *****************************************************************************************/
private:
	void PlayCurrentReloadMontage();
protected:
	void StartFire();
	void StopFire();

	void Reload();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerReload();
	UFUNCTION()
	void OnRep_Reload();

	UPROPERTY(ReplicatedUsing = OnRep_Reload, BlueprintReadOnly, Category = "Player")
	bool bIsReloading;
public:
	void SetIsReloading(bool bNewIsReloading);
	/******************************************************************************************
	  * Weapons
	  *****************************************************************************************/
protected:
	void SwitchToRifle();
	void SwitchToLauncher();
	void SwitchToLightningGun();

	UFUNCTION()
	void OnRep_CurrentWeapon(ASWeapon* PreviousWeapon);

	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon, BlueprintReadOnly, Category = "Weapon")
	ASWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TArray<TSubclassOf<ASWeapon>> WeaponList;

	UPROPERTY(Replicated)
	TArray<ASWeapon*> EquippedWeapons;
public:
	void SetCurrentWeapon(ASWeapon* NewWeapon, ASWeapon* PreviousWeapon);
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetCurrentWeapon(ASWeapon* NewWeapon, ASWeapon* PreviousWeapon);
	/******************************************************************************************
	  * Inventory
	  *****************************************************************************************/
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	TMap<EAmmoType, int32> MaxAmmoPerType;

	UPROPERTY(EditAnywhere, Category = "Ammo")
	TMap<EAmmoType, int32> DefaultAmmoPerType;

	UPROPERTY(Replicated, VisibleDefaultsOnly, Category = "Ammo")
	TArray<FAmmoInventory> CurrentAmmoPerType;
public:
	void AddAmmo(EAmmoType AmmoTypeToAdd, int32 AmmoAmountToAdd);

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentAmmoForType(EAmmoType AmmoType) const;

	UFUNCTION(BlueprintCallable)
	int32 GetMaxAmmoForType(EAmmoType AmmoType) const;

	/******************************************************************************************
	  * General components
	  *****************************************************************************************/
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComponent;

	/******************************************************************************************
	  * Mesh and socket names
	  *****************************************************************************************/
protected:
	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName;
};
