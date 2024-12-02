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

void APS_PlayerController::ReadyStartGame_Implementation(float RemainingTime)
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->SetStageTimer(RemainingTime);
        PS_HUD->ShowTimer();
    }
}

void APS_PlayerController::CancelStartGame_Implementation()
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->HideTimer();
    }
}

void APS_PlayerController::ShowWordSelectionUI_Implementation(float RemainingTime)
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->ToggleSelection();
        PS_HUD->SetSelectionTimer(RemainingTime);
    }
}

void APS_PlayerController::ShowWordSelectionWaitUI_Implementation(float RemainingTime)
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->ToggleSelectionWait();
        PS_HUD->SetSelectionWaitTimer(RemainingTime);
    }
}

void APS_PlayerController::ShowAnswerSelectionUI_Implementation(float RemainingTime)
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->ToggleAnswer();
        PS_HUD->SetAnswerTimer(RemainingTime);
    }
}

void APS_PlayerController::ShowAnswerSelectionWaitUI_Implementation(float RemainingTime)
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->ToggleAnswerWait();
        PS_HUD->SetAnswerWaitTimer(RemainingTime);
    }
}

void APS_PlayerController::SetStageTextUI(const FString& Text)
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->SetStageText(Text);
    }
}

void APS_PlayerController::ShowStageTimerUI_Implementation(float RemainingTime)
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->SetStageTimer(RemainingTime);
    }
}

void APS_PlayerController::HideStageTimerUI_Implementation()
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->HideTimer();
    }
}

void APS_PlayerController::ShowStageWordUI_Implementation(const FString& Answer)
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->ShowStageWord(Answer);
    }
}

void APS_PlayerController::HideStageWordUI_Implementation()
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->HideStageWord();
    }
}

void APS_PlayerController::SetSelectionButtonWords_Implementation(const TArray<FString>& SelectedWords)
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->SetSelectionButtonWords(SelectedWords);
    }

    for (const FString& Word : SelectedWords)
    {
        UE_LOG(Project_S, Log, TEXT("Button Word : %s\n"), *Word);
    }
}

void APS_PlayerController::SetAnswerSelectionButtonWords_Implementation(const TArray<FString>& SelectedWords)
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->SetAnswerSelectionButtonWords(SelectedWords);
    }
}

void APS_PlayerController::ToggleCorrectUI_Implementation(float RemainingTime, const FString& Answer, const FString& SelectedWord)
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->ToggleCorrectUI(RemainingTime, Answer, SelectedWord);
    }
}

void APS_PlayerController::ToggleWrongUI_Implementation(float RemainingTime, const FString& Answer, const FString& SelectedWord)
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->ToggleWrongUI(RemainingTime, Answer, SelectedWord);
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

