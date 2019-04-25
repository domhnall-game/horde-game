// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameState.h"

#include "Net/UnrealNetwork.h"

void ASGameState::OnRep_WaveState(EWaveState OldState)
{
	WaveStateChanged(WaveState, OldState);
}

void ASGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASGameState, WaveState);
}

void ASGameState::SetWaveState(EWaveState NewWaveState) {
	if (Role == ROLE_Authority) {
		EWaveState OldWaveState = WaveState;
		WaveState = NewWaveState;
		OnRep_WaveState(OldWaveState);
	}
}