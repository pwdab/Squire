// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Project_S.h"
#include "GameFramework/GameMode.h"
#include "Delegates/DelegateCombinations.h"
#include "PS_GameInstance.h"
#include "PS_GameState.h"
#include "PS_PlayerController.h"
#include "PS_PlayerState.h"
#include "PS_Words.h"
#include "PS_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_S_API APS_GameMode : public AGameMode
{
	GENERATED_BODY()

public:
	APS_GameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void PostSeamlessTravel() override;

	// �������� ��ȯ
	UFUNCTION(BlueprintCallable)
	void InitVariables();
	void OnHUDInitialized();

	UFUNCTION(BlueprintCallable)
	void TransitionToStage(uint8 MapNumber, uint8 StageNumber);

	// �κ�
	UFUNCTION(BlueprintCallable)
	void StartGameAfter5Seconds();
	UFUNCTION(BlueprintCallable)
	void ClearStartGameTimer();
	void OnStartGameAfter5SecondsComplete();

	// 1P�� �ܾ� ����
	void PostStartFirstWordSelectionTimer(int TimeLimit);
	void OnPostStartFirstWordSelectionTimerComplete();
	void StartFirstWordSelectionTimer(int TimeLimit);
	void OnFirstWordSelectionComplete();
	void StartFirstGameSession(int TimeLimit);
	void OnFirstGameSessionEnd();
	void StartFirstAnswerSelectionTimer(int TimeLimit);
	void OnFirstAnswerSelectionComplete();
	void FirstAnswerShow(int TimeLimit);
	void OnFirstAnswerShowComplete();

	// 2P�� �ܾ� ����
	void PostStartSecondWordSelectionTimer(int TimeLimit);
	void OnPostStartSecondWordSelectionTimerComplete();
	void StartSecondWordSelectionTimer(int TimeLimit);
	void OnSecondWordSelectionComplete();
	void StartSecondGameSession(int TimeLimit);
	void OnSecondGameSessionEnd();
	void StartSecondAnswerSelectionTimer(int TimeLimit);
	void OnSecondAnswerSelectionComplete();
	void SecondAnswerShow(int TimeLimit);
	void OnSecondAnswerShowComplete();

protected:
	virtual void BeginPlay() override;

private:
	/*
	UPS_GameInstance* PS_GameInstance;
	APS_GameState* PS_GameState;
	APS_PlayerController* PS_PlayerController_1;
	APS_PlayerController* PS_PlayerController_2;
	APS_PlayerState* PS_PlayerState_1;
	APS_PlayerState* PS_PlayerState_2;
	*/

	TArray<FString> InitializeWords(uint8 CurrentMap, uint8 CurrentStage, uint8 Num);

	// Data table reference variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<class UDataTable> WordDataTable;
	UDataTable* WordDataTable;

	// ��ü ���þ� ���
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Words", meta = (AllowPrivateAccess = "true"))
	TArray<FPS_Words> WordList;

	// ���� ���þ� ���
	UPROPERTY(BlueprintReadOnly, Category = "Words", meta = (AllowPrivateAccess = "true"))
	TArray<FString> UsedWords;

	// UI�� ����� 3���� ���þ� ���
	UPROPERTY(BlueprintReadOnly, Category = "Words", meta = (AllowPrivateAccess = "true"))
	TArray<FString> ButtonWords;

	// Ÿ�̸� �ð�
	const uint8 GameStartWaitTime = 5;
	const uint8 SelectionTime = 10;
	const uint8 GameSessionTime = 45;

	// Stage ����
	uint8 CurrentMap;
	uint8 CurrentStage;
	uint8 CurrentLife;

	uint8 CurrentPlayersCount;

	// Ÿ�̸� TimeHandle
	FTimerHandle StartGameTimerHandle;
	FTimerHandle SelectionUITimerHandle;
	FTimerHandle GameSessionTimerHandle;

	bool bIsGameStart;
	bool bIsCorrect;
	FString Answer;
	FString SelectedWord;

public:
	// Getter functions
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsGameStart() { return bIsGameStart; }
};