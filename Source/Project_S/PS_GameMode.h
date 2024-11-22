// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Project_S.h"
#include "GameFramework/GameMode.h"
#include "Delegates/DelegateCombinations.h"
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
	void StartWordSelectionTimer(int TimeLimit);
	void StartGameSession(int TimeLimit);

	void OnGameSessionEnd();
	void OnWordSelectionComplete();

protected:
	virtual void BeginPlay() override;

private:
	uint8 CurrentMap;
	uint8 CurrentStage;
	
	uint8 CurrentPlayersCount;

	FTimerHandle StageTransitionTimerHandle;
	FTimerHandle SelectionUITimerHandle;
	FTimerHandle GameSessionTimerHandle;
};
