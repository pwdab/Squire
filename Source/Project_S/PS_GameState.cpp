// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_GameState.h"
#include "PS_PlayerState.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

APS_GameState::APS_GameState()
{
    PrimaryActorTick.bCanEverTick = true;
    
    //RemainingTime = FTimespan::Zero();
    //TimerHandler.Invalidate();
    //bForward = true;

    CurrentMap = 1;
    CurrentStage = 1;
    CurrentLife = 2;
}

void APS_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // Setup Replication variables
    //DOREPLIFETIME(APS_GameState, RemainingTime);
    DOREPLIFETIME(APS_GameState, CurrentMap);
    DOREPLIFETIME(APS_GameState, CurrentStage);
    DOREPLIFETIME(APS_GameState, CurrentLife);
}

void APS_GameState::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    /*
    if (!TimerHandler.IsValid())
    {
        UpdateTimerbyMiliSecond();
    }

    GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Blue, FString::Printf(TEXT("MatchState = %s"), *GetMatchState().ToString()));
    */
}

void APS_GameState::BeginPlay()
{
    Super::BeginPlay();

    /*
    SetTimer(0, 0, 0, 0);

    StartTimer(true);

    OnMapChanged.Broadcast(Map);
    OnStageChanged.Broadcast(Stage);
    OnLifeChanged.Broadcast(Life);
    */
}

void APS_GameState::HandleMatchHasStarted()
{
    /*
    PS_LOG_S(Log);
    for (APlayerState* PlayerState : PlayerArray)
    {
        if (PlayerState)
        {
            UE_LOG(Project_S, Log, TEXT("Player: %s"), *PlayerState->GetPlayerName());
        }
    }
    */
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

void APS_GameState::SetStage(int MapNumber, int StageNumber)
{
    PS_LOG_S(Log);
    CurrentMap = MapNumber;
    CurrentStage = StageNumber;

    OnMapChanged.Broadcast(CurrentMap);
    OnStageChanged.Broadcast(CurrentStage);
}

/*
FTimespan APS_GameState::GetRemainingTime() const
{
    return RemainingTime;
}
*/

void APS_GameState::DeductLife()
{
    PS_LOG_S(Log);
    CurrentLife--;

    OnLifeChanged.Broadcast(CurrentLife);

    if (CurrentLife <= 0)
    {
        // Handle game over logic
    }
}

bool APS_GameState::AllPlayersSelected() const
{
    PS_LOG_S(Log);
    for (APlayerState* PlayerState : PlayerArray)
    {
        APS_PlayerState* PS_PlayerState = Cast<APS_PlayerState>(PlayerState);
        if (PS_PlayerState && !PS_PlayerState->HasSelectedWord())
        {
            return false;
        }
    }
    return true;
}
