// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Project_S.h"
#include "GameFramework/GameMode.h"
#include "Delegates/DelegateCombinations.h"
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

	// 로비 및 스테이지 전환 관리
	void TransitionToStage(uint8 MapNumber ,uint8 StageNumber);

	// 1P가 단어 선택
	void StartFirstWordSelectionTimer(int TimeLimit);
	void OnFirstWordSelectionComplete();
	void StartFirstGameSession(int TimeLimit);
	void OnFirstGameSessionEnd();
	void StartFirstAnswerSelectionTimer(int TimeLimit);
	void OnFirstAnswerSelectionComplete();

	// 2P가 단어 선택
	void StartSecondWordSelectionTimer(int TimeLimit);
	void OnSecondWordSelectionComplete();
	void StartSecondGameSession(int TimeLimit);
	void OnSecondGameSessionEnd();
	void StartSecondAnswerSelectionTimer(int TimeLimit);
	void OnSecondAnswerSelectionComplete();
	

protected:
	virtual void BeginPlay() override;

private:
	TArray<FString> InitializeWords(uint8 CurrentMap, uint8 CurrentStage, uint8 Num);

	// Data table reference variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<class UDataTable> WordDataTable;
	UDataTable* WordDataTable;

	// 전체 제시어 목록
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Words", meta = (AllowPrivateAccess = "true"))
	TArray<FPS_Words> WordList;

	// 사용된 제시어 목록
	UPROPERTY(BlueprintReadOnly, Category = "Words", meta = (AllowPrivateAccess = "true"))
	TArray<FString> UsedWords;

	// UI에 출력할 3개의 제시어 목록
	UPROPERTY(BlueprintReadOnly, Category = "Words", meta = (AllowPrivateAccess = "true"))
	TArray<FString> ButtonWords;

	const uint8 SelectionTime = 5;
	const uint8 GameSessionTime = 5;

	uint8 CurrentMap;
	uint8 CurrentStage;
	uint8 CurrentLife;
	
	uint8 CurrentPlayersCount;

	FTimerHandle SelectionUITimerHandle;	// 단어 선택 TimerHandler
	FTimerHandle GameSessionTimerHandle;

public:
	// Getter functions
	//FORCEINLINE uint8 GetCurrentPlayersCount() const { return CurrentPlayersCount; }

public:
	void OnHUDInitialized();
};
