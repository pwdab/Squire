// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_PlayerController.h"
#include "PS_PlayerState.h"
#include "PS_GameMode.h"
#include "PS_HUD.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

void APS_PlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (GEngine)
    {
        // OnNetworkFailure 이벤트 바인딩
        GEngine->OnNetworkFailure().AddUObject(this, &APS_PlayerController::HandleNetworkFailure);
    }
}

void APS_PlayerController::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
    UE_LOG(Project_S, Warning, TEXT("Network Failure Detected: %s"), *ErrorString);

    /*
    // 네트워크 실패 유형에 따른 처리
    if (FailureType == ENetworkFailure::FailureReceived || FailureType == ENetworkFailure::ConnectionLost)
    {
        UE_LOG(Project_S, Warning, TEXT("Handling Network Failure: Returning to Main Menu"));

        // 세션 종료 처리
        IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
        if (OnlineSubsystem)
        {
            IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
            if (SessionInterface.IsValid())
            {
                SessionInterface->DestroySession(NAME_GameSession);
            }
        }
    }
    */
    DestroyCurrentSession();
}

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

void APS_PlayerController::ShowStageTextUI_Implementation(const FString& Text)
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->SetStageText(Text);
    }
}

void APS_PlayerController::HideStageTextUI_Implementation()
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->HideStageText();
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

void APS_PlayerController::ShowGameOverUI_Implementation(bool IsServer)
{
    PS_LOG_S(Log);
    APS_HUD* PS_HUD = Cast<APS_HUD>(GetHUD());
    if (PS_HUD)
    {
        PS_HUD->ToggleGameOverUI(IsServer);
    }
}

void APS_PlayerController::TransitionToStage_Implementation()
{
    PS_LOG_S(Log);
    APS_GameMode* PS_GameMode = Cast<APS_GameMode>(GetWorld()->GetAuthGameMode());
    if (PS_GameMode)
    {
        PS_GameMode->TransitionToStage(0, 0);
    }
}

void APS_PlayerController::TransitionToMainMenu()
{
    PS_LOG_S(Log);
    
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            // 세션 종료
            SessionInterface->DestroySession(NAME_GameSession, FOnDestroySessionCompleteDelegate::CreateUObject(this, &APS_PlayerController::OnDestroySessionComplete));
        }
    }
    else
    {
        // 세션 없이 직접 이동
        ClientTravel(TEXT("/Game/Maps/Level_MainMenu"), ETravelType::TRAVEL_Absolute);
    }
}

void APS_PlayerController::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    PS_LOG_S(Log);

    if (bWasSuccessful)
    {
        UE_LOG(Project_S, Log, TEXT("Session destroyed successfully. Transitioning to MainMenu."));
    }
    else
    {
        UE_LOG(Project_S, Warning, TEXT("Failed to destroy session. Transitioning to MainMenu anyway."));
    }

    // 메인 메뉴로 이동
    ClientTravel(TEXT("/Game/Maps/Level_MainMenu"), ETravelType::TRAVEL_Absolute);
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

