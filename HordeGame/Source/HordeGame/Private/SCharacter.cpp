// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"

#include "SWeapon.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"

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

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultFOV = CameraComponent->FieldOfView;

	//Spawn a default weapon
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
		CurrentWeapon = EquippedWeapons[0];
		CurrentWeapon->SetActorHiddenInGame(false);
	}

	//CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(WeaponList[0].Get(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	//if (CurrentWeapon) {
	//	CurrentWeapon->SetOwner(this);
	//	CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
	//}
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
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);

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

void ASCharacter::StartFire()
{
	if (CurrentWeapon) {
		CurrentWeapon->StartFire();
	}
}

void ASCharacter::StopFire()
{
	if (CurrentWeapon) {
		CurrentWeapon->StopFire();
	}
}

void ASCharacter::SwitchToRifle()
{
	/*
	CurrentWeapon->Destroy();
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(WeaponList[0].Get(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (CurrentWeapon) {
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
	}
	*/
	StopFire();
	CurrentWeapon->SetActorHiddenInGame(true);
	CurrentWeapon = EquippedWeapons[0];
	CurrentWeapon->SetActorHiddenInGame(false);
}

void ASCharacter::SwitchToLauncher()
{
	/*
	CurrentWeapon->Destroy();
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(WeaponList[1].Get(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (CurrentWeapon) {
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
	}
	*/
	StopFire();
	CurrentWeapon->SetActorHiddenInGame(true);
	CurrentWeapon = EquippedWeapons[1];
	CurrentWeapon->SetActorHiddenInGame(false);
}

FVector ASCharacter::GetPawnViewLocation() const
{
	if (ensure(CameraComponent)) {
		return CameraComponent->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

