// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Project_S.h"
#include "GameFramework/GameState.h"
#include "Delegates/DelegateCombinations.h"
#include "PS_GameState.generated.h"

/**
 * 
 */

// Declare Delegates
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeChanged, FTimespan, NewTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapChanged, uint8, NewMap);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStageChanged, uint8, NewStage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLifeChanged, uint8, NewLife);

UCLASS()
class PROJECT_S_API APS_GameState : public AGameState
{
	GENERATED_BODY()
	
public:
    APS_GameState();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    void SetStage(int MapNumber, int StageNumber);
    void DeductLife();
    bool AllPlayersSelected() const;

    // Declare Delegate Functions
    /*
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnTimeChanged OnTimeChanged;
    */

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnMapChanged OnMapChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnStageChanged OnStageChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnLifeChanged OnLifeChanged;

    /*
    UFUNCTION(BlueprintCallable, Category = "Timer")
    FTimespan GetRemainingTime() const;

    UFUNCTION(BlueprintCallable, Category = "Timer")
    void SetTimer(int Hours, int Minutes, int Seconds, int Miliseconds);

    UFUNCTION(BlueprintCallable, Category = "Timer")
    void PauseTimer();

    UFUNCTION(BlueprintCallable, Category = "Timer")
    void ClearTimer();

    UFUNCTION(BlueprintCallable, Category = "Timer")
    void StartTimer(bool Forward);

    UFUNCTION(BlueprintCallable, Category = "Timer")
    void UpdateTimerbyMiliSecond();

    UFUNCTION(BlueprintCallable, Category = "Timer")
    void OnTimerEnd();
    */

protected:
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;


    virtual void HandleMatchHasStarted() override;

    

    // Replicate Variables
    /*
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing="OnRep_Time", Category = "HUD")
    FTimespan RemainingTime;
    */

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = "OnRep_Map", Category = "HUD")
    uint8 CurrentMap;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = "OnRep_Stage", Category = "HUD")
    uint8 CurrentStage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = "OnRep_Life", Category = "HUD")
    uint8 CurrentLife;

    /*
    UFUNCTION()
    void OnRep_Time(FTimespan OldValue) const;
    */

    UFUNCTION()
    void OnRep_Map(uint8 OldValue) const;
    
    UFUNCTION()
    void OnRep_Stage(uint8 OldValue) const;

    UFUNCTION()
    void OnRep_Life(uint8 OldValue) const;

public:
    // Getter functions
    FORCEINLINE uint8 GetCurrentMap() const { return CurrentMap; }
    FORCEINLINE uint8 GetCurrentStage() const { return CurrentStage; }
    FORCEINLINE uint8 GetCurrentLife() const { return CurrentLife; }
};