// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"

#include "HordeGame.h"
#include "SAmmoInventory.h"
#include "SHealthComponent.h"
#include "SWeapon.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComponent->FieldOfView;

	for (const TPair<EAmmoType, int32>& DefaultAmmoPair : DefaultAmmoPerType) {
		FAmmoInventory AmmoInventory;
		AmmoInventory.AmmoType = DefaultAmmoPair.Key;
		AmmoInventory.AmmoAmount = DefaultAmmoPair.Value;
		CurrentAmmoPerType.Insert(AmmoInventory, (uint32) DefaultAmmoPair.Key);
	}

	//Spawn a default weapon
	if (Role == ROLE_Authority) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if (WeaponList.Num() > 0) {
			for (int i = 0; i < WeaponList.Num(); i++) {
				ASWeapon* Weapon = GetWorld()->SpawnActor<ASWeapon>(WeaponList[i].Get(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
				Weapon->SetOwner(this);
				Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
				Weapon->SetActorHiddenInGame(true);
				EquippedWeapons.Add(Weapon);
			}
			SetCurrentWeapon(EquippedWeapons[ID_WEAP_RIFLE], nullptr);
		}
	}

	HealthComponent->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	HealthComponent = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	WeaponAttachSocketName = "weapon_socket";

	MaxAmmoPerType = ASAmmoInventory::InitializedMaxAmmoMap();
	DefaultAmmoPerType = ASAmmoInventory::InitializedDefaultAmmoMap();

	bIsDead = false;
	bIsReloading = false;
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Aim(DeltaTime);
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookPitch", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookYaw", this, &ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ASCharacter::StartAim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ASCharacter::EndAim);

	PlayerInputComponent->BindAction("SwitchToRifle", IE_Pressed, this, &ASCharacter::SwitchToRifle);
	PlayerInputComponent->BindAction("SwitchToLauncher", IE_Pressed, this, &ASCharacter::SwitchToLauncher);
	PlayerInputComponent->BindAction("SwitchToLightningGun", IE_Pressed, this, &ASCharacter::SwitchToLightningGun);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ASCharacter::Reload);
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, CurrentWeapon);
	DOREPLIFETIME(ASCharacter, EquippedWeapons);
	DOREPLIFETIME(ASCharacter, CurrentAmmoPerType);
	DOREPLIFETIME_CONDITION(ASCharacter, bIsDead, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ASCharacter, bIsReloading, COND_SkipOwner);
}

void ASCharacter::OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	//Death
	if (Health <= 0.0f && !bIsDead) {
		bIsDead = true;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();
		SetLifeSpan(10.f);
	}
}

void ASCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ASCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ASCharacter::BeginCrouch()
{
	Crouch();
}

void ASCharacter::EndCrouch()
{
	UnCrouch();
}

void ASCharacter::StartAim()
{
	bIsAiming = true;
}

void ASCharacter::EndAim()
{
	bIsAiming = false;
}

void ASCharacter::Aim(float DeltaTime)
{
	float TargetFOV = bIsAiming ? AimDownSightsFOV : DefaultFOV;
	float NewFOV = FMath::FInterpTo(CameraComponent->FieldOfView, TargetFOV, DeltaTime, AimInterpSpeed);
	CameraComponent->SetFieldOfView(NewFOV);
}

FVector ASCharacter::GetPawnViewLocation() const
{
	if (ensure(CameraComponent)) {
		return CameraComponent->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

void ASCharacter::PlayCurrentReloadMontage() {
	UAnimMontage* ReloadMontage = CurrentWeapon->GetReloadMontage();
	if (ReloadMontage) {
		PlayAnimMontage(ReloadMontage);
	}
}

void ASCharacter::StartFire()
{
	if (!CurrentWeapon) {
		UE_LOG(LogTemp, Warning, TEXT("does not have a current weapon"));
	}
	if (CurrentWeapon && !bIsReloading) {
		CurrentWeapon->StartFire();
	} else if (CurrentWeapon && bIsReloading) {
		//UE_LOG(LogTemp, Warning, TEXT("SCharacter -- Character is reloading, cannot currently fire"));
	}
}

void ASCharacter::StopFire()
{
	if (CurrentWeapon) {
		CurrentWeapon->StopFire();
	}
}

void ASCharacter::Reload()
{
	if (Role < ROLE_Authority) {
		ServerReload();
	}

	if (!bIsReloading) {
		bIsReloading = true;
		StopFire();

		if (CurrentWeapon) {
			EAmmoType CurrentWeaponAmmoType = CurrentWeapon->GetAmmoType();
			FAmmoInventory CurrentAmmoInventory = CurrentAmmoPerType[(uint32) CurrentWeaponAmmoType];
			int32 CurrentAmmo = CurrentAmmoInventory.AmmoAmount;
			int32 ReloadedAmmo = CurrentWeapon->Reload(CurrentAmmo);
			CurrentAmmoInventory.AmmoAmount = CurrentAmmo - ReloadedAmmo;
			CurrentAmmoPerType[(uint8)CurrentWeaponAmmoType] = CurrentAmmoInventory;

			PlayCurrentReloadMontage();
		}
	}
}

void ASCharacter::ServerReload_Implementation()
{
	Reload();
}

bool ASCharacter::ServerReload_Validate()
{
	return true;
}

void ASCharacter::OnRep_Reload()
{
	if (bIsReloading) {
		PlayCurrentReloadMontage();
	}
}

void ASCharacter::SetIsReloading(bool bNewIsReloading)
{
	bIsReloading = bNewIsReloading;
}

void ASCharacter::SwitchToRifle()
{
	SetCurrentWeapon(EquippedWeapons[ID_WEAP_RIFLE], CurrentWeapon);
}

void ASCharacter::SwitchToLauncher()
{
	SetCurrentWeapon(EquippedWeapons[ID_WEAP_LAUNCHER], CurrentWeapon);
}

void ASCharacter::SwitchToLightningGun()
{
	SetCurrentWeapon(EquippedWeapons[ID_WEAP_LIGHTNING], CurrentWeapon);
}

void ASCharacter::OnRep_CurrentWeapon(ASWeapon* PreviousWeapon)
{
	SetCurrentWeapon(CurrentWeapon, PreviousWeapon);
}

void ASCharacter::SetCurrentWeapon(ASWeapon* NewWeapon, ASWeapon* PreviousWeapon)
{
	if (NewWeapon == PreviousWeapon) {
		return;
	}

	//I don't fully understand it, but calling ServerSetCurrentWeapon with "Role < ROLE_Authority" appears to try to change the other players' weapons when a client changes his?
	//It doesn't succeed in changing other players weapons, but it tries to
	//Calling it with this role check fixes that; it now seems to only call ServerSetCurrentWeapon for the player actually doing the weapon swap
	if (Role == ROLE_AutonomousProxy) {
		ServerSetCurrentWeapon(NewWeapon, PreviousWeapon);
	}

	if (!bIsReloading) {
		StopFire();

		if (PreviousWeapon) {
			PreviousWeapon->SetActorHiddenInGame(true);
		}
		if (NewWeapon) {
			NewWeapon->SetActorHiddenInGame(false);
			CurrentWeapon = NewWeapon;
		}
	}
}

void ASCharacter::ServerSetCurrentWeapon_Implementation(ASWeapon* NewWeapon, ASWeapon* PreviousWeapon)
{
	SetCurrentWeapon(NewWeapon, PreviousWeapon);
}

bool ASCharacter::ServerSetCurrentWeapon_Validate(ASWeapon* NewWeapon, ASWeapon* PreviousWeapon)
{
	return true;
}

void ASCharacter::AddAmmo(EAmmoType AmmoTypeToAdd, int32 AmmoAmountToAdd)
{
	FAmmoInventory AmmoInventory = CurrentAmmoPerType[(uint8) AmmoTypeToAdd];
	int32 CurrentAmount = AmmoInventory.AmmoAmount;
	int32 MaxAmount = *MaxAmmoPerType.Find(AmmoTypeToAdd);
	AmmoInventory.AmmoAmount = FMath::Min(CurrentAmount + AmmoAmountToAdd, MaxAmount);
	CurrentAmmoPerType[(uint8) AmmoTypeToAdd] = AmmoInventory;
}

int32 ASCharacter::GetCurrentAmmoForType(EAmmoType AmmoType) const
{
	return CurrentAmmoPerType[(uint8)AmmoType].AmmoAmount;
}

int32 ASCharacter::GetMaxAmmoForType(EAmmoType AmmoType) const
{
	return *MaxAmmoPerType.Find(AmmoType);
}
