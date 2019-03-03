// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AmmoTypeEnum.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class UDamageType;

//Contains information of a single hitscan weapon line trace
USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
	FVector_NetQuantize TraceTo;

	//We don't actually care about this property and it is never checked
	//However, if we update it every time we update anything else in the struct, it forces Unreal to send it anyway
	//This fixes a bug wherein effects would not play if a character fires at the same location multiple times in a row
	//UE would not resend the struct because it hadn't changed; with this variable changed, it will now resend the struct
	UPROPERTY()
	uint8 BurstCounter;
};

UCLASS()
class HORDEGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Fire();

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerFire();

	UFUNCTION()
	void OnRep_HitScanTrace();

	void PlayFireEffects(FVector ParticleEndVector);
	void PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Sockets")
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Sockets")
	FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	UParticleSystem* TracerEffect;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	TSubclassOf<UCameraShake> FireCamShake;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageMultiplier;

	FTimerHandle TimerHandle_AutoFireDelay;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float AutoFireDelay;

	float LastFireTime;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	EAmmoType AmmoType;

	int32 CurrentAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 MaxLoadedAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 MaxRange;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UAnimMontage* ReloadMontage;

	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;
public:
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	virtual void StartFire();
	virtual void StopFire();
	virtual int32 Reload(int32 ReloadAmount);

	EAmmoType GetAmmoType() { return AmmoType; }
	int32 GetMaxLoadedAmmo() { return MaxLoadedAmmo; }
	UAnimMontage* GetReloadMontage() { return ReloadMontage; }
};
