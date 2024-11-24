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

    // UI를 통해 단어 선택
    UFUNCTION(Client, Reliable)
    void ShowWordSelectionUI();

    UFUNCTION(Server, Reliable)
    void ServerHUDInitialized();

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
    FString SelectedWord;
};
