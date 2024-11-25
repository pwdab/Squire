// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_PlayerController.h"
#include "PS_PlayerState.h"
#include "PS_GameMode.h"
#include "PS_HUD.h"
#include "Blueprint/UserWidget.h"

void APS_PlayerController::ServerHUDInitialized_Implementation()
{
    if (APS_GameMode* PS_GameMode = Cast<APS_GameMode>(GetWorld()->GetAuthGameMode()))
    {
        PS_GameMode->OnHUDInitialized();
    }
}

void APS_PlayerController::ShowWordSelectionUI_Implementation(FTimerHandle TimerHandle)
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->ToggleSelection();
        //FTimespan Timespan = FTimespan::FromSeconds(GetWorldTimerManager().GetTimerRemaining(TimerHandle));
        PS_HUD->SetSelectionTimer(TimerHandle);
    }
}

void APS_PlayerController::ShowWordSelectionWaitUI_Implementation(FTimerHandle TimerHandle)
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->ToggleSelectionWait();
        PS_HUD->SetSelectionWaitTimer(TimerHandle);
    }
}

void APS_PlayerController::ShowAnswerSelectionUI_Implementation(FTimerHandle TimerHandle)
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->ToggleAnswer();
        //FTimespan Timespan = FTimespan::FromSeconds(GetWorldTimerManager().GetTimerRemaining(TimerHandle));
        PS_HUD->SetAnswerTimer(TimerHandle);
    }
}

void APS_PlayerController::ShowAnswerSelectionWaitUI_Implementation(FTimerHandle TimerHandle)
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->ToggleAnswerWait();
        PS_HUD->SetAnswerWaitTimer(TimerHandle);
    }
}

void APS_PlayerController::ShowStageTimerUI_Implementation(FTimerHandle TimerHandle)
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->SetStageTimer(TimerHandle);
    }
}

void APS_PlayerController::HideStageTimerUI_Implementation()
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->ClearStageTimer();
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

