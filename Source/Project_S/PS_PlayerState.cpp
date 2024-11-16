// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_PlayerState.h"

void APS_PlayerState::UpdateSelectedWord(FString Word)
{
	PS_LOG_S(Log);
	SelectedWord = Word;
	bHasSelectedWord = true;
}

bool APS_PlayerState::HasSelectedWord() const
{
	PS_LOG_S(Log);
	return bHasSelectedWord;
}