// Fill out your copyright notice in the Description page of Project Settings.

#include "PS_GameMode.h"
#include "PS_GameState.h"
#include "PS_PlayerController.h"
#include "PS_PlayerState.h"
#include "PS_Character.h"
#include "UObject/ConstructorHelpers.h"


APS_GameMode::APS_GameMode()
{
	GameStateClass = APS_GameState::StaticClass();
	PlayerStateClass = APS_PlayerState::StaticClass();
	PlayerControllerClass = APS_PlayerController::StaticClass();

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_Character"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}