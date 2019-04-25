// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

enum class EWaveState : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController);

/**
 * 
 */
UCLASS()
class HORDEGAME_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	int32 WaveCount;
	//Bots to spawn in single wave
	int32 NumberOfBotsToSpawn;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenWaves;

	FTimerHandle TimerHandle_BotSpawner;
	FTimerHandle TimerHandle_NextWaveStart;
		
protected:
	//Hook for BP to spawn a single bot
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewBot();

	void SpawnBotTimerElapsed();

	//Start spawning bots
	void StartWave();
	//Stop spawning bots
	void EndWave();
	//Set timer for next wave start
	void PrepareForNextWave();
	void CheckWaveState();
	void CheckAnyPlayerAlive();
	void GameOver();
	void SetWaveState(EWaveState NewState);

public:
	ASGameMode();
	virtual void StartPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;
};
