// Fill out your copyright notice in the Description page of Project Settings.

#include "PS_GameMode.h"
#include "PS_GameState.h"
#include "PS_PlayerController.h"
#include "PS_PlayerState.h"
#include "PS_GameInstance.h"
#include "PS_Character.h"
#include "PS_HUD.h"
#include "UObject/ConstructorHelpers.h"


APS_GameMode::APS_GameMode()
{
	GameStateClass = APS_GameState::StaticClass();
	PlayerStateClass = APS_PlayerState::StaticClass();
	//PlayerControllerClass = APS_PlayerController::StaticClass();

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_Character"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

    static ConstructorHelpers::FClassFinder<APS_HUD> HUDBPClass(TEXT("/Game/Blueprints/BP_HUD"));
    if (HUDBPClass.Class != nullptr)
    {
        HUDClass = HUDBPClass.Class;
    }

    static ConstructorHelpers::FClassFinder<APS_PlayerController> PlayerControllerBPClass(TEXT("/Game/Blueprints/BP_PlayerController"));
    if (PlayerControllerBPClass.Class != nullptr)
    {
        PlayerControllerClass = PlayerControllerBPClass.Class;
    }

    CurrentPlayersCount = 0;
    bUseSeamlessTravel = true;
}

void APS_GameMode::BeginPlay()
{
    Super::BeginPlay(); 

    if (UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance()))
    {
        CurrentMap = PS_GameInstance->GetMap();
        CurrentStage = PS_GameInstance->GetStage();
    }

    PS_LOG_S(Log);
    
    APS_GameState* PS_GameState = GetGameState<APS_GameState>();
    if (PS_GameState)
    {
        PS_GameState->SetStage(CurrentMap, CurrentStage);
        //PS_GameState->CurrentHUDCount = 0;
    }
}

void APS_GameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    PS_LOG_S(Log);
    UE_LOG(Project_S, Log, TEXT("Player has joined the game: %s"), *NewPlayer->GetName());
}

void APS_GameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);
    //CurrentPlayersCount--;
}

void APS_GameMode::TransitionToStage(uint8 MapNumber, uint8 StageNumber)
{
    PS_LOG_S(Log);
    // 맵 이름을 생성하고 맵을 로드
    FString MapName = FString::Printf(TEXT("/Game/Maps/Level_%d_%d?listen"), MapNumber, StageNumber);

    CurrentPlayersCount = 0;
    //GetWorld()->ServerTravel(MapName, true);
    GetWorld()->ServerTravel(TEXT("/Game/Maps/Level_0_new?listen"), true);
}

void APS_GameMode::PostSeamlessTravel()
{
    Super::PostSeamlessTravel();

    PS_LOG_S(Log);

    APS_GameState* PS_GameState = GetGameState<APS_GameState>();
    if (PS_GameState)
    {
        for (APlayerState* PlayerState : PS_GameState->PlayerArray)
        {
            if (PlayerState)
            {
                UE_LOG(Project_S, Log, TEXT("Player %s has joined the new map."), *PlayerState->GetPlayerName());
            }
        }
    }

    if (UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance()))
    {
        CurrentMap = PS_GameInstance->GetMap();
        CurrentStage = PS_GameInstance->GetStage();

        PS_GameState->SetStage(CurrentMap, CurrentStage);
        PS_GameState->SetLife(PS_GameInstance->GetLife());
    }
}

void APS_GameMode::OnHUDInitialized()
{
    CurrentPlayersCount++;
    UE_LOG(Project_S, Log, TEXT("CurrentPlayersCount = %d\n"), CurrentPlayersCount);

    if (CurrentPlayersCount == 2)
    {
        StartFirstWordSelectionTimer(SelectionTime);
    }
}

void APS_GameMode::StartFirstWordSelectionTimer(int TimeLimit)
{
    PS_LOG_S(Log);
    /*
    // GameState의 bAllPlayerSelected 초기화
    APS_GameState* PS_GameState = GetGameState<APS_GameState>();
    if (PS_GameState)
    {
        PS_GameState->SetSelection(false);
    }
    */

    // 타이머 설정
    GetWorldTimerManager().SetTimer(SelectionUITimerHandle, this, &APS_GameMode::OnFirstWordSelectionComplete, TimeLimit, false);

    // 첫번째 PlayerController에 단어 선택 UI 추가
    FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();
    APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowWordSelectionUI(SelectionUITimerHandle);
    }

    // 두번째 PlayerController에 대기 UI 추가
    It++;
    PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowWordSelectionWaitUI(SelectionUITimerHandle);
    }
}

void APS_GameMode::OnFirstWordSelectionComplete()
{
    PS_LOG_S(Log);

    // 첫번째 PlayerController에 단어 선택 UI 제거
    FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();
    APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowWordSelectionUI(SelectionUITimerHandle);
    }

    // 첫번째 PlayerController의 bHasSelectedWord가 true인지 검사
    APS_PlayerState* PS_PlayerState = Cast<APS_PlayerState>(PS_PlayerController->PlayerState);
    if (PS_PlayerState)
    {
        // 선택하지 않았으면 랜덤으로 선택
        if (!PS_PlayerState->GetHasSelectedWord())
        {
            PS_PlayerState->UpdateSelectedWord_Server(FString(TEXT("Not Selected.. \n")));
        }
    }

    // 두번째 PlayerController에 대기 UI 제거
    It++;
    PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowWordSelectionWaitUI(SelectionUITimerHandle);
    }

    /*
    APS_GameState* PS_GameState = GetGameState<APS_GameState>();
    // GameState의 bAllPlayerSelected가 true인지 검사
    if (PS_GameState)
    {
        PS_GameState->AllPlayersWordSelected();

        if (PS_GameState->GetCurrentSelection())
        {
            StartGameSession(120); // Start 2 min game session
        }
        else
        {
            // Move to Next Level
            CurrentStage++;
            if (CurrentStage > 10)
            {
                CurrentMap++;
                CurrentStage -= 10;
            }

            if (UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance()))
            {
                PS_GameInstance->SetMap(CurrentMap);
                PS_GameInstance->SetStage(CurrentStage);
                PS_GameInstance->DeductLife();
            }

            //TransitionToStage(CurrentMap, CurrentStage);
        }
    }
    */

    // 게임 시작
    StartFirstGameSession(GameSessionTime);
}

void APS_GameMode::StartFirstGameSession(int TimeLimit)
{
    PS_LOG_S(Log);
    GetWorldTimerManager().SetTimer(GameSessionTimerHandle, this, &APS_GameMode::OnFirstGameSessionEnd, TimeLimit, false);

    // 모든 Player에 Timer UI 추가
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
    {
        APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
        if (PS_PlayerController)
        {
            PS_PlayerController->ShowStageTimerUI(GameSessionTimerHandle);
        }
    }
}

void APS_GameMode::OnFirstGameSessionEnd()
{
    PS_LOG_S(Log);
    // 모든 Player에 Timer UI 제거
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
    {
        APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
        if (PS_PlayerController)
        {
            PS_PlayerController->HideStageTimerUI();
        }
    }
    StartFirstAnswerSelectionTimer(SelectionTime);
}

void APS_GameMode::StartFirstAnswerSelectionTimer(int TimeLimit)
{
    PS_LOG_S(Log);

    // 타이머 설정
    GetWorldTimerManager().SetTimer(SelectionUITimerHandle, this, &APS_GameMode::OnFirstAnswerSelectionComplete, TimeLimit, false);

    // 첫번째 PlayerController에 대기 UI 추가
    FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();
    APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowAnswerSelectionWaitUI(SelectionUITimerHandle);
    }

    // 두번째 PlayerController에 정답 선택 UI 추가
    It++;
    PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowAnswerSelectionUI(SelectionUITimerHandle);
    }
}

void APS_GameMode::OnFirstAnswerSelectionComplete()
{
    PS_LOG_S(Log);

    // 첫번째 PlayerController에 대기 UI 제거
    FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();
    APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowAnswerSelectionWaitUI(SelectionUITimerHandle);
    }

    // 두번째 PlayerController에 정답 선택 UI 제거
    It++;
    PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowAnswerSelectionUI(SelectionUITimerHandle);
    }

    // 두번째 PlayerController의 bHasSelectedWord가 true인지 검사
    APS_PlayerState* PS_PlayerState = Cast<APS_PlayerState>(PS_PlayerController->PlayerState);
    if (PS_PlayerState)
    {
        UE_LOG(Project_S, Log, TEXT("Player 2 has selected : %s"), *PS_PlayerState->GetSelectedWord());
        // 정답
        if (PS_PlayerState->GetHasSelectedWord())
        {
            UE_LOG(Project_S, Log, TEXT("Player 2 has selected : %s"), *PS_PlayerState->GetSelectedWord());
        }
        // 오답
        else
        {
            if (UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance()))
            {
                PS_GameInstance->DeductLife();
            }
        }

        CurrentStage++;
        if (CurrentStage > 10)
        {
            CurrentMap++;
            CurrentStage -= 10;

            if (UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance()))
            {
                PS_GameInstance->SetMap(CurrentMap);
                PS_GameInstance->SetStage(CurrentStage);

                TransitionToStage(CurrentMap, CurrentStage);
            }
        }
        else
        {
            if (UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance()))
            {
                PS_GameInstance->SetMap(CurrentMap);
                PS_GameInstance->SetStage(CurrentStage);

                StartSecondWordSelectionTimer(SelectionTime);
            }
        }
    }
}

void APS_GameMode::StartSecondWordSelectionTimer(int TimeLimit)
{
    PS_LOG_S(Log);

    // 타이머 설정
    GetWorldTimerManager().SetTimer(SelectionUITimerHandle, this, &APS_GameMode::OnSecondWordSelectionComplete, TimeLimit, false);

    // 첫번째 PlayerController에 대기 UI 추가
    FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();
    APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowWordSelectionWaitUI(SelectionUITimerHandle);
    }

    // 두번째 PlayerController에 단어 선택 UI 추가
    It++;
    PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowWordSelectionUI(SelectionUITimerHandle);
    }
}

void APS_GameMode::OnSecondWordSelectionComplete()
{
    PS_LOG_S(Log);

    // 첫번째 PlayerController에 대기 UI 제거
    FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();
    APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowWordSelectionWaitUI(SelectionUITimerHandle);
    }

    // 두번째 PlayerController에 단어 선택 UI 제거
    It++;
    PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowWordSelectionUI(SelectionUITimerHandle);
    }

    // 두번째 PlayerController의 bHasSelectedWord가 true인지 검사
    APS_PlayerState* PS_PlayerState = Cast<APS_PlayerState>(PS_PlayerController->PlayerState);
    if (PS_PlayerState)
    {
        // 선택하지 않았으면 랜덤으로 선택
        if (!PS_PlayerState->GetHasSelectedWord())
        {
            PS_PlayerState->UpdateSelectedWord_Server(FString(TEXT("Not Selected.. \n")));
        }
    }

    // 게임 시작
    StartSecondGameSession(GameSessionTime);
}

void APS_GameMode::StartSecondGameSession(int TimeLimit)
{
    PS_LOG_S(Log);
    GetWorldTimerManager().SetTimer(GameSessionTimerHandle, this, &APS_GameMode::OnSecondGameSessionEnd, TimeLimit, false);

    // 모든 Player에 Timer UI 추가
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
    {
        APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
        if (PS_PlayerController)
        {
            PS_PlayerController->ShowStageTimerUI(GameSessionTimerHandle);
        }
    }
}

void APS_GameMode::OnSecondGameSessionEnd()
{
    PS_LOG_S(Log);
    // 모든 Player에 Timer UI 제거
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
    {
        APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
        if (PS_PlayerController)
        {
            PS_PlayerController->HideStageTimerUI();
        }
    }
    StartSecondAnswerSelectionTimer(SelectionTime);
}

void APS_GameMode::StartSecondAnswerSelectionTimer(int TimeLimit)
{
    PS_LOG_S(Log);

    // 타이머 설정
    GetWorldTimerManager().SetTimer(SelectionUITimerHandle, this, &APS_GameMode::OnSecondAnswerSelectionComplete, TimeLimit, false);

    // 첫번째 PlayerController에 정답 선택 UI 추가
    FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();
    APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowAnswerSelectionUI(SelectionUITimerHandle);
    }

    // 두번째 PlayerController에 대기 UI 추가
    It++;
    PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowAnswerSelectionWaitUI(SelectionUITimerHandle);
    }
}

void APS_GameMode::OnSecondAnswerSelectionComplete()
{
    PS_LOG_S(Log);

    // 첫번째 PlayerController에 정답 선택 UI 제거
    FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();
    APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowAnswerSelectionUI(SelectionUITimerHandle);
    }

    // 두번째 PlayerController에 대기 UI 제거
    It++;
    PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowAnswerSelectionWaitUI(SelectionUITimerHandle);
    }

    // 첫번째 PlayerController의 bHasSelectedWord가 true인지 검사
    PS_PlayerController = Cast<APS_PlayerController>(GetWorld()->GetPlayerControllerIterator()->Get());
    APS_PlayerState* PS_PlayerState = Cast<APS_PlayerState>(PS_PlayerController->PlayerState);
    if (PS_PlayerState)
    {
        // 정답
        if (PS_PlayerState->GetHasSelectedWord())
        {
            PS_PlayerState->UpdateSelectedWord_Server(FString(TEXT("Not Selected.. \n")));
        }
        // 오답
        else
        {
            if (UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance()))
            {
                PS_GameInstance->DeductLife();
            }
        }

        CurrentStage++;
        if (CurrentStage > 10)
        {
            CurrentMap++;
            CurrentStage -= 10;

            if (UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance()))
            {
                PS_GameInstance->SetMap(CurrentMap);
                PS_GameInstance->SetStage(CurrentStage);

                TransitionToStage(CurrentMap, CurrentStage);
            }
        }
        else
        {
            if (UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance()))
            {
                PS_GameInstance->SetMap(CurrentMap);
                PS_GameInstance->SetStage(CurrentStage);
            }

            StartFirstWordSelectionTimer(SelectionTime);
        }
    }
}

/*
void APS_GameMode::OnWordSelectionComplete()
{
    PS_LOG_S(Log);

    // 모든 PlayerController에 단어 선택 UI 제거
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
        if (PS_PlayerController)
        {
            PS_PlayerController->ShowWordSelectionUI(SelectionUITimerHandle);
        }
    }

    APS_GameState* PS_GameState = GetGameState<APS_GameState>();
    // GameState의 bAllPlayerSelected가 true인지 검사
    if (PS_GameState)
    {
        PS_GameState->AllPlayersSelected();

        if (PS_GameState->GetCurrentSelection())
        {
            StartGameSession(120); // Start 2 min game session
        }
        else
        {
            // Move to Next Level
            CurrentStage++;
            if (CurrentStage > 10)
            {
                CurrentMap++;
                CurrentStage -= 10;
            }

            if (UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance()))
            {
                PS_GameInstance->SetMap(CurrentMap);
                PS_GameInstance->SetStage(CurrentStage);
                PS_GameInstance->DeductLife();
            }

            //TransitionToStage(CurrentMap, CurrentStage);
        }
    }
}
*/

