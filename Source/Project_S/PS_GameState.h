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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectionChanged, bool, NewBool);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStartChanged, bool, NewBool);

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemainingTimeChanged, float, NewRemainingTime);

UCLASS()
class PROJECT_S_API APS_GameState : public AGameState
{
	GENERATED_BODY()
	
public:
    APS_GameState();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    void UpdateGameState();
    void SetStage(int MapNumber, int StageNumber);
    void SetLife(int NewLife);
    void SetSelection(bool NewSelection);
    void SetGameStart(bool NewGameStart);
    //void SetRemainingTime(float NewRemainingTime);

    UFUNCTION(Client, Reliable)
    void AllPlayersWordSelected();

    UFUNCTION(Client, Reliable)
    void AllPlayersAnswerSelected();

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

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnSelectionChanged OnSelectionChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnGameStartChanged OnGameStartChanged;

    /*
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnRemainingTimeChanged OnRemainingTimeChanged;
    */
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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = "OnRep_Selection", Category = "HUD")
    bool CurrentSelection;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = "OnRep_GameStart", Category = "HUD")
    bool GameStart;

    /*
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = "OnRep_RemainingTime", Category = "HUD")
    float RemainingTime;
    */

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

    UFUNCTION()
    void OnRep_Selection(bool OldValue) const;

    UFUNCTION()
    void OnRep_GameStart(bool OldValue) const;

    /*
    UFUNCTION()
    void OnRep_RemainingTime(float OldValue) const;
    */

public:
    // Getter functions
    FORCEINLINE uint8 GetCurrentMap() const { return CurrentMap; }
    FORCEINLINE uint8 GetCurrentStage() const { return CurrentStage; }
    FORCEINLINE uint8 GetCurrentLife() const { return CurrentLife; }
    FORCEINLINE bool GetCurrentSelection() const { return CurrentSelection; }
    FORCEINLINE bool GetGameStart() const { return GameStart; }
    //FORCEINLINE float GetRemainingTime() { return RemainingTime; }
};