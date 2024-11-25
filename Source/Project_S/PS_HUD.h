// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Project_S.h"
#include "GameFramework/HUD.h"
#include "PS_HUD.generated.h"

UCLASS()
class PROJECT_S_API APS_HUD : public AHUD
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD")
    void UpdateVariables();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD")
    void ToggleDebug();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD")
    void ToggleSelection();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD")
    void ToggleSelectionWait();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD")
    void ToggleAnswer();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD")
    void ToggleAnswerWait();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD")
    void SetSelectionTimer(FTimerHandle TimerHandle);

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD")
    void SetSelectionWaitTimer(FTimerHandle TimerHandle);

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD")
    void SetAnswerTimer(FTimerHandle TimerHandle);

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD")
    void SetAnswerWaitTimer(FTimerHandle TimerHandle);

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD")
    void SetStageTimer(FTimerHandle TimerHandle);

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD")
    void ClearStageTimer();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
    APS_GameState* PS_GameState;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
    uint8 CurrentMap;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
    uint8 CurrentStage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
    uint8 CurrentLife;

    void OnMapChanged(uint8 NewMap);
    void OnStageChanged(uint8 NewStage);
    void OnLifeChanged(uint8 NewLife);
};