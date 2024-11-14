// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Project_S.h"
#include "GameFramework/PlayerState.h"
#include "PS_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_S_API APS_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	void UpdateSelectedWord(FString Word);
	bool HasSelectedWord() const;

	UPROPERTY()
	FString SelectedWord;

	bool bHasSelectedWord;
};
