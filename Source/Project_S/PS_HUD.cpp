// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_HUD.h"
#include "PS_GameState.h"
#include "BaseWidgetBlueprint.h"

void APS_HUD::BeginPlay()
{
	PS_LOG_S(Log);
	Super::BeginPlay();

	PS_GameState = Cast<APS_GameState>(GetWorld()->GetGameState());

	if (PS_GameState)
	{
		UE_LOG(Project_S, Log, TEXT("Update Map, Stage, Life\n"));
		CurrentMap = PS_GameState->GetCurrentMap();
		CurrentStage = PS_GameState->GetCurrentStage();
		CurrentLife = PS_GameState->GetCurrentLife();
	}
	else
	{
		UE_LOG(Project_S, Log, TEXT("PS_GameState is null\n"));
	}

	UpdateVariables();

	// Setup Delegates
	PS_GameState->OnMapChanged.AddDynamic(this, &APS_HUD::OnMapChanged);
	PS_GameState->OnStageChanged.AddDynamic(this, &APS_HUD::OnStageChanged);
	PS_GameState->OnLifeChanged.AddDynamic(this, &APS_HUD::OnLifeChanged);
}



void APS_HUD::OnMapChanged(uint8 NewMap)
{
	PS_LOG_S(Log);
	CurrentMap = PS_GameState->GetCurrentMap();
}

void APS_HUD::OnStageChanged(uint8 NewStage)
{
	PS_LOG_S(Log);
	CurrentStage = PS_GameState->GetCurrentStage();
}

void APS_HUD::OnLifeChanged(uint8 NewLife)
{
	PS_LOG_S(Log);
	CurrentLife = PS_GameState->GetCurrentLife();
}