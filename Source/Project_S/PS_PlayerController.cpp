// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_PlayerController.h"
#include "PS_PlayerState.h"
#include "PS_GameMode.h"
#include "PS_HUD.h"
#include "Blueprint/UserWidget.h"

void APS_PlayerController::ShowWordSelectionUI_Implementation()
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->ToggleSelection();
    }
    else
    {
        UE_LOG(Project_S, Log, TEXT("PS_HUD is Null\n"));
    }
}

void APS_PlayerController::OnSelectWord(FString Word)
{
    PS_LOG_S(Log);
    APS_PlayerState* PS_PlayerState = GetPlayerState<APS_PlayerState>();
    if (PS_PlayerState)
    {
        PS_PlayerState->UpdateSelectedWord(Word);
    }
    WordSelectionWidget->RemoveFromViewport();
}

void APS_PlayerController::ServerHUDInitialized_Implementation()
{
    if (APS_GameMode* PS_GameMode = Cast<APS_GameMode>(GetWorld()->GetAuthGameMode()))
    {
        PS_GameMode->OnHUDInitialized();
    }
}