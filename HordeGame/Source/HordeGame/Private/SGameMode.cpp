// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameMode.h"

#include "TimerManager.h"

ASGameMode::ASGameMode()
{
	WaveCount = 0;
	TimeBetweenWaves = 2.f;
}

void ASGameMode::StartWave()
{
	WaveCount++;
	NumberOfBotsToSpawn = 2 * WaveCount;
	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &ASGameMode::SpawnBotTimerElapsed, 1.f, true, 0.f);
}

void ASGameMode::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	NumberOfBotsToSpawn--;

	if (NumberOfBotsToSpawn <= 0) {
		EndWave();
	}
}

void ASGameMode::StartPlay()
{
	Super::StartPlay();
	PrepareForNextWave();
}

void ASGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);
	PrepareForNextWave();
}

void ASGameMode::PrepareForNextWave()
{
	FTimerHandle TimerHandle_NextWaveStart;
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ASGameMode::StartWave, TimeBetweenWaves, false);
}
