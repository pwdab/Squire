// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_GameState.h"
#include "Net/UnrealNetwork.h"

APS_GameState::APS_GameState()
{
    PrimaryActorTick.bCanEverTick = true;
    
    RemainingTime = FTimespan::Zero();
    TimerHandler.Invalidate();
    bForward = true;

    Map = 1;
    Stage = 1;
    Life = 2;
}

void APS_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // Setup Replication variables
    DOREPLIFETIME(APS_GameState, RemainingTime);
    DOREPLIFETIME(APS_GameState, Map);
    DOREPLIFETIME(APS_GameState, Stage);
    DOREPLIFETIME(APS_GameState, Life);
}

void APS_GameState::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!TimerHandler.IsValid())
    {
        UpdateTimerbyMiliSecond();
    }
}

void APS_GameState::BeginPlay()
{
    Super::BeginPlay();

    SetTimer(0, 1, 0, 0);

    StartTimer(true);

    OnMapChanged.Broadcast(Map);
    OnStageChanged.Broadcast(Stage);
    OnLifeChanged.Broadcast(Life);
}

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

void APS_GameState::OnRep_Map(uint8 OldValue) const
{
    OnMapChanged.Broadcast(Map);
}

void APS_GameState::OnRep_Stage(uint8 OldValue) const
{
    OnStageChanged.Broadcast(Stage);
}

void APS_GameState::OnRep_Life(uint8 OldValue) const
{
    OnLifeChanged.Broadcast(Life);
}