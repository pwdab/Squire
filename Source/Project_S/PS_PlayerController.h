// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Project_S.h"
#include "GameFramework/PlayerController.h"
#include "PS_PlayerController.generated.h"

// Forward declaration for UserWidget class
class UUserWidget;

/**
 * 
 */
UCLASS()
class PROJECT_S_API APS_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    void OnEnterWordSelectionZone();
    void OnExitWordSelectionZone();
    void OnSelectWord(FString Word);

    UFUNCTION(Server, Reliable)
    void ServerHUDInitialized();

    // Start Game
    UFUNCTION(Client, Reliable)
    void ReadyStartGame(float RemainingTime);

    UFUNCTION(Client, Reliable)
    void CancelStartGame();

    // UI
    UFUNCTION(Client, Reliable)
    void ShowWordSelectionUI(float RemainingTime);

    UFUNCTION(Client, Reliable)
    void ShowWordSelectionWaitUI(float RemainingTime);

    UFUNCTION(Client, Reliable)
    void ShowAnswerSelectionUI(float RemainingTime);

    UFUNCTION(Client, Reliable)
    void ShowAnswerSelectionWaitUI(float RemainingTime);

    UFUNCTION(Client, Reliable)
    void ShowStageTextUI(const FString& Text);

    UFUNCTION(Client, Reliable)
    void HideStageTextUI();

    UFUNCTION(Client, Reliable)
    void ShowStageTimerUI(float RemainingTime);

    UFUNCTION(Client, Reliable)
    void HideStageTimerUI();

    UFUNCTION(Client, Reliable)
    void ShowStageWordUI(const FString& Answer);

    UFUNCTION(Client, Reliable)
    void HideStageWordUI();

    UFUNCTION(Client, Reliable)
    void SetSelectionButtonWords(const TArray<FString>& SelectedWords);

    UFUNCTION(Client, Reliable)
    void SetAnswerSelectionButtonWords(const TArray<FString>& SelectedWords);

    UFUNCTION(Client, Reliable)
    void ToggleCorrectUI(float RemainingTime, const FString& Answer, const FString& SelectedWord);

    UFUNCTION(Client, Reliable)
    void ToggleWrongUI(float RemainingTime, const FString& Answer, const FString& SelectedWord);

    UFUNCTION(Client, Reliable)
    void ShowGameOverUI(bool IsServer);

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void TransitionToStage();

protected:
    // Word selection widget class, settable in editor
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> WordSelectionWidgetClass;

    // Instance of the word selection widget
    UPROPERTY()
    UUserWidget* WordSelectionWidget;

private:
    bool bIsInZone;
    bool bHasSelectedWord;
};
