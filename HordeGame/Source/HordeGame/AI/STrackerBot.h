// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

UCLASS()
class HORDEGAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector GetNextPathPoint();

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Tracking")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "Tracking")
	float RequiredDistanceToTarget;

	UPROPERTY(EditDefaultsOnly, Category = "Tracking")
	bool bUseVelocityChange;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;	
};
