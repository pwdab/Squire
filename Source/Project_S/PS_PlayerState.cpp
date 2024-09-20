// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_PlayerState.h"
#include "PS_Character.h"
#include "Net/UnrealNetwork.h"

/*
void APS_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APS_PlayerState, Velocity, COND_OwnerOnly);
}

void APS_PlayerState::UpdateVelocity(const float Value)
{
	Velocity = Value;
	OnVelocityChanged.Broadcast(Velocity);

	GEngine->AddOnScreenDebugMessage(6, 5.f, FColor::Yellow, FString::Printf(TEXT("New Velocity: %f"), Value));
}

void APS_PlayerState::OnRep_Velocity(float OldValue) const
{
	OnVelocityChanged.Broadcast(Velocity);
}
*/