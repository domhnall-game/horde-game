// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameMode.h"

#include "SGameState.h"
#include "SPlayerState.h"
#include "SHealthComponent.h"

#include "TimerManager.h"
#include "Engine/World.h"

ASGameMode::ASGameMode()
{
	PlayerStateClass = ASPlayerState::StaticClass();
	GameStateClass = ASGameState::StaticClass();

	WaveCount = 0;
	TimeBetweenWaves = 2.f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f;
}

void ASGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CheckWaveState();
	CheckAnyPlayerAlive();
}

void ASGameMode::StartPlay()
{
	Super::StartPlay();
	PrepareForNextWave();
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

void ASGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);
	//PrepareForNextWave();
	SetWaveState(EWaveState::WaitingToComplete);
}

void ASGameMode::CheckWaveState()
{
	//Do not run this function if we are still in a wave, or if we are currently preparing for a wave
	if (NumberOfBotsToSpawn > 0 || GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart)) {
		return;
	}

	bool bIsAnyBotAlive = false;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It) {
		APawn* TestPawn = It->Get();
		if (TestPawn == nullptr || TestPawn->IsPlayerControlled()) {
			continue;
		}

		USHealthComponent* HealthComponent = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (HealthComponent && HealthComponent->GetCurrentHealth() > 0.f) {
			bIsAnyBotAlive = true;
			break;
		}
	}

	//If all bots have been killed, begin next wave preparation
	if (!bIsAnyBotAlive) {
		PrepareForNextWave();
	}
}

void ASGameMode::CheckAnyPlayerAlive()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It) {
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn()) {
			APawn* ControllerPawn = PC->GetPawn();
			USHealthComponent* HealthComponent = Cast<USHealthComponent>(ControllerPawn->GetComponentByClass(USHealthComponent::StaticClass()));

			if (ensure(HealthComponent) && HealthComponent->GetCurrentHealth() > 0.f) {
				return;
			}
		}
	}

	//No player alive
	GameOver();
}

void ASGameMode::GameOver()
{
	EndWave();
	//@TODO: Finish up game and present stats to players

	UE_LOG(LogTemp, Warning, TEXT("GAME OVER: All players dead"));
	SetWaveState(EWaveState::GameOver);
}

void ASGameMode::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ASGameMode::StartWave, TimeBetweenWaves, false);
	SetWaveState(EWaveState::WaitingToStart);
}

void ASGameMode::SetWaveState(EWaveState NewState)
{
	ASGameState* GameState = GetGameState<ASGameState>();
	if (ensureAlways(GameState)) {
		GameState->SetWaveState(NewState);
	}
}
