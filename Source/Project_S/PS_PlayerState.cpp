// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_PlayerState.h"
#include "Net/UnrealNetwork.h"

void APS_PlayerState::BeginPlay()
{
	PS_LOG_S(Log);
}

void APS_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Setup Replication variables
	DOREPLIFETIME(APS_PlayerState, SelectedWord);
	DOREPLIFETIME(APS_PlayerState, bHasSelectedWord);
}

void APS_PlayerState::OnRep_Word(FString OldValue) const
{
	OnWordChanged.Broadcast(SelectedWord);
}

void APS_PlayerState::OnRep_WordSelected(bool OldValue) const
{
	OnWordSelected.Broadcast(bHasSelectedWord);
}

void APS_PlayerState::UpdateSelectedWord_Implementation(const FString& Word)
{
	PS_LOG_S(Log);
	UpdateSelectedWord_Server(Word);
}

void APS_PlayerState::UpdateSelectedWord_Server_Implementation(const FString& Word)
{
	PS_LOG_S(Log);
	UE_LOG(Project_S, Log, TEXT("Selected Word : %s"), *Word);
	SelectedWord = Word;
	bHasSelectedWord = true;

	OnWordChanged.Broadcast(SelectedWord);
	OnWordSelected.Broadcast(bHasSelectedWord);
}

bool APS_PlayerState::HasSelectedWord() const
{
	PS_LOG_S(Log);
	if (bHasSelectedWord)
	{
		UE_LOG(Project_S, Log, TEXT("PlayerState Owner : %s, Selected : true\n"), *GetOwner()->GetName());
	}
	else
	{
		UE_LOG(Project_S, Log, TEXT("PlayerState Owner : %s, Selected : false\n"), *GetOwner()->GetName());
	}
	
	return bHasSelectedWord;
}