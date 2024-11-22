// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_HUD.h"
#include "PS_GameMode.h"
#include "PS_GameState.h"
#include "PS_PlayerController.h"
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
		/*
		// Setup Delegates
		PS_GameState->OnMapChanged.AddDynamic(this, &APS_HUD::OnMapChanged);
		PS_GameState->OnStageChanged.AddDynamic(this, &APS_HUD::OnStageChanged);
		PS_GameState->OnLifeChanged.AddDynamic(this, &APS_HUD::OnLifeChanged);
		*/
		//PS_GameState->CurrentHUDCount++;
		PS_GameState->SetHUD(PS_GameState->GetCurrentHUD() + 1);
	}
	else
	{
		UE_LOG(Project_S, Log, TEXT("GameState is null\n"));
	}
	
	UpdateVariables();

	if (APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(GetOwningPlayerController()))
	{
		PS_PlayerController->ServerHUDInitialized();
	}

	/*
	if (PS_GameState->CurrentHUDCount == 2)
	{
		UE_LOG(Project_S, Log, TEXT("CurrentHUDCount = 2\n"));
		APS_GameMode* PS_GameMode = Cast<APS_GameMode>(GetWorld()->GetAuthGameMode());
		if (PS_GameMode)
		{
			PS_GameMode->StartWordSelectionTimer(10);
		}
		else
		{
			UE_LOG(Project_S, Log, TEXT("PS_GameMode is null\n"));
		}
	}
	else
	{
		UE_LOG(Project_S, Log, TEXT("CurrentHUDCount = %d\n"), PS_GameState->CurrentHUDCount);
	}

	if (PS_GameState->GetCurrentHUD() == 2)
	{
		UE_LOG(Project_S, Log, TEXT("CurrentHUD = 2\n"));
		APS_GameMode* PS_GameMode = Cast<APS_GameMode>(GetWorld()->GetAuthGameMode());
		if (PS_GameMode)
		{
			PS_GameMode->StartWordSelectionTimer(10);
		}
		else
		{
			UE_LOG(Project_S, Log, TEXT("PS_GameMode is null\n"));
		}
	}
	else
	{
		UE_LOG(Project_S, Log, TEXT("CurrentHUD = %d\n"), PS_GameState->GetCurrentHUD());
	}
	*/
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