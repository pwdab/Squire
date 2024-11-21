// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_GameInstance.h"

UPS_GameInstance::UPS_GameInstance()
{
    Map = 0;
    Stage = 0;
    Life = 2;
}

void UPS_GameInstance::SetMap(uint8 InMap)
{
    Map = InMap;
    PS_LOG_S(Log);
    UE_LOG(Project_S, Log, TEXT("Map = %d\n"), Map);
}

void UPS_GameInstance::SetStage(uint8 InStage)
{
    Stage = InStage;
    PS_LOG_S(Log);
    UE_LOG(Project_S, Log, TEXT("Stage = %d\n"), Stage);
}

void UPS_GameInstance::SetLife(uint8 InLife)
{
    Life = InLife;
}

void UPS_GameInstance::DeductLife()
{
    Life--;
    PS_LOG_S(Log);
    UE_LOG(Project_S, Log, TEXT("Life = %d\n"), Life);
}

uint8 UPS_GameInstance::GetMap() const
{
    return Map;
}

uint8 UPS_GameInstance::GetStage() const
{
    return Stage;
}

uint8 UPS_GameInstance::GetLife() const
{
    return Life;
}