// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_PlayerController.h"
#include "PS_PlayerState.h"
#include "Blueprint/UserWidget.h"

void APS_PlayerController::ShowWordSelectionUI()
{
    PS_LOG_S(Log);
    if (WordSelectionWidgetClass && !WordSelectionWidget)
    {
        WordSelectionWidget = CreateWidget<UUserWidget>(this, WordSelectionWidgetClass);
        if (WordSelectionWidget)
        {
            WordSelectionWidget->AddToViewport();
        }
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