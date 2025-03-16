// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_PlayerState.h"
#include "Net/UnrealNetwork.h"

void APS_PlayerState::BeginPlay()
{
	PS_LOG_S(Log);
	if (GetOwner())
	{
		UE_LOG(Project_S, Log, TEXT("PlayerState Owner : %s\n"), *GetOwner()->GetName());
	}
	else
	{
		UE_LOG(Project_S, Log, TEXT("PlayerState Owner is null\n"));
	}
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

void APS_PlayerState::InitSelectedWord_Implementation()
{
	PS_LOG_S(Log);
	InitSelectedWord_Server();
}

void APS_PlayerState::InitSelectedWord_Server_Implementation()
{
	PS_LOG_S(Log);

	SelectedWord = "";
	bHasSelectedWord = false;

	OnWordChanged.Broadcast(SelectedWord);
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
	
	SelectedWord = Word;
	bHasSelectedWord = true;

	UE_LOG(Project_S, Log, TEXT("Selected Word : %s"), *Word);
	if (bHasSelectedWord)
	{
		UE_LOG(Project_S, Log, TEXT("bHasSelectedWord : true"));
	}
	else
	{
		UE_LOG(Project_S, Log, TEXT("bHasSelectedWord : false"));
	}

	OnWordChanged.Broadcast(SelectedWord);
	OnWordSelected.Broadcast(bHasSelectedWord);
}

FString APS_PlayerState::GetSelectedWord_Implementation()
{
	return SelectedWord;
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