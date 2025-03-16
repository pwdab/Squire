// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_GameState.h"
#include "PS_GameMode.h"
#include "PS_PlayerState.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

APS_GameState::APS_GameState()
{
    PrimaryActorTick.bCanEverTick = true;
    
    //RemainingTime = FTimespan::Zero();
    //TimerHandler.Invalidate();
    //bForward = true;

    CurrentSelection = false;
}

void APS_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // Setup Replication variables
    //DOREPLIFETIME(APS_GameState, RemainingTime);
    DOREPLIFETIME(APS_GameState, CurrentMap);
    DOREPLIFETIME(APS_GameState, CurrentStage);
    DOREPLIFETIME(APS_GameState, CurrentLife);
    DOREPLIFETIME(APS_GameState, CurrentSelection);
    DOREPLIFETIME(APS_GameState, GameStart);
    //DOREPLIFETIME(APS_GameState, RemainingTime);
}

/*
FTimespan APS_GameState::GetRemainingTime() const
{
    return RemainingTime;
}

void APS_GameState::SetTimer(int Hours, int Minutes, int Seconds, int Miliseconds)
{
    //UE_LOG(Project_S, Log, TEXT("Set Time to : %2d:%2d:%2d:%3d\n"), Hours, Minutes, Seconds, Miliseconds);
    RemainingTime = FTimespan(0, Hours, Minutes, Seconds, Miliseconds);
}

void APS_GameState::PauseTimer()
{
    if (TimerHandler.IsValid())
    {
        GetWorld()->GetTimerManager().PauseTimer(TimerHandler);
    }
}

void APS_GameState::ClearTimer()
{
    GetWorld()->GetTimerManager().ClearTimer(TimerHandler);
    RemainingTime = FTimespan::Zero();
    TimerHandler.Invalidate();
}

void APS_GameState::StartTimer(bool Forward)
{
    bForward = Forward;

    if (TimerHandler.IsValid())
    {
        GetWorld()->GetTimerManager().UnPauseTimer(TimerHandler);
    }
    else
    {
        GetWorld()->GetTimerManager().SetTimer(TimerHandler, this, &APS_GameState::UpdateTimerbyMiliSecond, 0.001f, true);
    }
}

void APS_GameState::UpdateTimerbyMiliSecond()
{
    if (bForward)
    {
        RemainingTime += FTimespan::FromMilliseconds(1);
        OnTimeChanged.Broadcast(RemainingTime);
    }
    else
    {
        RemainingTime -= FTimespan::FromMilliseconds(1);
        OnTimeChanged.Broadcast(RemainingTime);

        // 타이머 종료 조건
        if (RemainingTime <= FTimespan::Zero())
        {
            OnTimerEnd();
            ClearTimer();
        }
    }
}

void APS_GameState::OnTimerEnd()
{
    UE_LOG(LogTemp, Warning, TEXT("Timer has ended!"));
}

void APS_GameState::OnRep_Time(FTimespan OldValue) const
{
    OnTimeChanged.Broadcast(RemainingTime);
}
*/

void APS_GameState::OnRep_Map(uint8 OldValue) const
{
    OnMapChanged.Broadcast(CurrentMap);
}

void APS_GameState::OnRep_Stage(uint8 OldValue) const
{
    OnStageChanged.Broadcast(CurrentStage);
}

void APS_GameState::OnRep_Life(uint8 OldValue) const
{
    OnLifeChanged.Broadcast(CurrentLife);
}

void APS_GameState::OnRep_Selection(bool OldValue) const
{
    OnSelectionChanged.Broadcast(CurrentSelection);
}

void APS_GameState::OnRep_GameStart(bool OldValue) const
{
    OnGameStartChanged.Broadcast(GameStart);
}

/*
void APS_GameState::OnRep_RemainingTime(float OldValue) const
{
    PS_LOG_S(Log);
    OnRemainingTimeChanged.Broadcast(RemainingTime);
}
*/

void APS_GameState::UpdateGameState()
{
    PS_LOG_S(Log);
    UE_LOG(Project_S, Log, TEXT("Map - Stage = %d - %d, Life = %d\n"), CurrentMap, CurrentStage, CurrentLife);

    OnMapChanged.Broadcast(CurrentMap);
    OnStageChanged.Broadcast(CurrentStage);
    OnLifeChanged.Broadcast(CurrentLife);
}

void APS_GameState::SetStage(int MapNumber, int StageNumber)
{
    CurrentMap = MapNumber;
    CurrentStage = StageNumber;
    OnMapChanged.Broadcast(CurrentMap);
    OnStageChanged.Broadcast(CurrentStage);
}

void APS_GameState::SetLife(int NewLife)
{
    CurrentLife = NewLife;
    OnLifeChanged.Broadcast(CurrentLife);
}

void APS_GameState::SetSelection(bool NewSelection)
{
    CurrentSelection = NewSelection;
    OnSelectionChanged.Broadcast(CurrentSelection);
}

void APS_GameState::SetGameStart(bool NewGameStart)
{
    GameStart = NewGameStart;
    OnGameStartChanged.Broadcast(GameStart);
}

/*
void APS_GameState::SetRemainingTime(float NewRemainingTime)
{
    PS_LOG_S(Log);
    RemainingTime = NewRemainingTime;
    OnRemainingTimeChanged.Broadcast(RemainingTime);
    UE_LOG(Project_S, Log, TEXT("RemainingTime = %f"), RemainingTime);
}
*/
/*
FTimespan APS_GameState::GetRemainingTime() const
{
    return RemainingTime;
}
*/

void APS_GameState::AllPlayersWordSelected_Implementation()
{
    bool temp = true;
    for (APlayerState* PlayerState : PlayerArray)
    {
        APS_PlayerState* PS_PlayerState = Cast<APS_PlayerState>(PlayerState);
        if (PS_PlayerState && !PS_PlayerState->HasSelectedWord())
        {
            temp = false;
        }
    }
    SetSelection(temp);
}

void APS_GameState::AllPlayersAnswerSelected_Implementation()
{
    bool temp = true;
    for (APlayerState* PlayerState : PlayerArray)
    {
        APS_PlayerState* PS_PlayerState = Cast<APS_PlayerState>(PlayerState);
        if (PS_PlayerState && !PS_PlayerState->HasSelectedWord())
        {
            temp = false;
        }
    }
    SetSelection(temp);
}