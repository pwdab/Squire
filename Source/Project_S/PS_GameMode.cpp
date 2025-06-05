// Fill out your copyright notice in the Description page of Project Settings.

#include "PS_GameMode.h"
#include "PS_GameState.h"
#include "PS_PlayerController.h"
#include "PS_PlayerState.h"
#include "PS_GameInstance.h"
#include "PS_Character.h"
#include "PS_HUD.h"
#include "PS_Words.h"
#include "Algo/RandomShuffle.h"
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
    bIsGameStart = false;

    static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("/Game/Blueprints/WordDataTable"));
    if (DataTable.Succeeded())
    {
        WordDataTable = DataTable.Object;
    }
    else
    {
        UE_LOG(Project_S, Error, TEXT("Failed to load WordDataTable."));
    }

    UE_LOG(Project_S, Warning, TEXT(" "));
    UE_LOG(Project_S, Warning, TEXT(" "));
    UE_LOG(Project_S, Warning, TEXT(" "));
    UE_LOG(Project_S, Warning, TEXT(" "));
    UE_LOG(Project_S, Warning, TEXT("Debug.. Game Started"));
    UE_LOG(Project_S, Warning, TEXT(" "));
    UE_LOG(Project_S, Warning, TEXT(" "));
    UE_LOG(Project_S, Warning, TEXT(" "));
    UE_LOG(Project_S, Warning, TEXT(" "));
}

void APS_GameMode::BeginPlay()
{
    Super::BeginPlay(); 

    PS_LOG_S(Log);

    if (UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance()))
    {
        CurrentMap = PS_GameInstance->GetMap();
        CurrentStage = PS_GameInstance->GetStage();
        CurrentLife = PS_GameInstance->GetLife();
        bIsGameStart = PS_GameInstance->IsGameStart();
    }

    GetWorldTimerManager().ClearTimer(StartGameTimerHandle);
    GetWorldTimerManager().ClearTimer(SelectionUITimerHandle);
    GetWorldTimerManager().ClearTimer(GameSessionTimerHandle);

    if (APS_GameState* PS_GameState = GetGameState<APS_GameState>())
    {
        PS_GameState->SetStage(CurrentMap, CurrentStage);
        PS_GameState->SetLife(CurrentLife);
        PS_GameState->SetGameStart(bIsGameStart);
        //PS_GameState->SetRemainingTime(0);
    }

    // 사용된 제시어 초기화
    UsedWords.Empty();
}

TArray<FString> APS_GameMode::InitializeWords(uint8 Map, uint8 Stage, uint8 Num)
{
    PS_LOG_S(Log);

    uint8 Difficulty;
    if (1 <= Stage && Stage < 4)
    {
        Difficulty = 1;
    }
    else if (4 <= Stage && Stage < 8)
    {
        Difficulty = 2;
    }
    else
    {
        Difficulty = 3;
    }
    TArray<FString> AvailableWords;
    TArray<FString> SelectedWords;

    if (WordDataTable)
    {
        TArray<FName> RowNames = WordDataTable->GetRowNames();
        WordList.Empty();

        // 데이터 테이블의 모든 행 가져오기
        for (const FName& RowName : RowNames)
        {
            // 데이터 테이블의 특정 행 가져오기
            FPS_Words* RowData = WordDataTable->FindRow<FPS_Words>(RowName, TEXT("InitializeWordData"));
            if (RowData)
            {
                // 필터 조건에 맞는지 확인
                if (RowData->MapNum == Map && RowData->Difficulty == Difficulty)
                {
                    WordList.Add(*RowData);
                    //UE_LOG(Project_S, Log, TEXT("Filtered MapNum : %d, Difficulty : %d, Word : %s"), RowData->MapNum, RowData->Difficulty, *RowData->Word);
                }
            }
        }

        // 중복되지 않는 단어만 AvailableWords에 추가
        for (const FPS_Words& Word : WordList)
        {
            if (!UsedWords.Contains(Word.Word))
            {
                AvailableWords.Add(Word.Word);
            }
        }

        // 랜덤으로 Num개만큼 선택
        while (SelectedWords.Num() < Num)
        {
            int32 RandomIndex = FMath::RandRange(0, AvailableWords.Num() - 1);
            SelectedWords.Add(AvailableWords[RandomIndex]);
            //UsedWords.Add(AvailableWords[RandomIndex]);
            AvailableWords.RemoveAt(RandomIndex);
        }
    }

    return SelectedWords;
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
    // 로비로 이동
    if (MapNumber == 0)
    {
        CurrentMap = 1;
        CurrentStage = 1;
        CurrentLife = 3;
        bIsGameStart = false;
        if (UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance()))
        {
            PS_GameInstance->SetMap(CurrentMap);
            PS_GameInstance->SetStage(CurrentStage);
            PS_GameInstance->SetLife(CurrentLife);
            PS_GameInstance->SetIsGameStart(bIsGameStart);
        }
        if (APS_GameState* PS_GameState = GetGameState<APS_GameState>())
        {
            PS_GameState->SetStage(CurrentMap, CurrentStage);
            PS_GameState->SetLife(CurrentLife);
            PS_GameState->SetGameStart(bIsGameStart);
        }
        GetWorld()->ServerTravel(TEXT("/Game/Maps/Level_Test?listen"), true);
    }
    else
    {
        if (UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance()))
        {
            PS_GameInstance->SetMap(CurrentMap);
            PS_GameInstance->SetStage(CurrentStage);
            PS_GameInstance->SetLife(CurrentLife);
            PS_GameInstance->SetIsGameStart(bIsGameStart);
        }
        if (APS_GameState* PS_GameState = GetGameState<APS_GameState>())
        {
            PS_GameState->SetStage(CurrentMap, CurrentStage);
            PS_GameState->SetLife(CurrentLife);
            PS_GameState->SetGameStart(bIsGameStart);
        }
        //GetWorld()->ServerTravel(MapName, true);
        GetWorld()->ServerTravel(TEXT("/Game/Maps/Level_0_new?listen"), true);
    }
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

    UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance());
    if (PS_GameInstance)
    {
        CurrentMap = PS_GameInstance->GetMap();
        CurrentStage = PS_GameInstance->GetStage();
        CurrentLife = PS_GameInstance->GetLife();
        bIsGameStart = PS_GameInstance->IsGameStart();

        PS_GameState->SetStage(CurrentMap, CurrentStage);
        PS_GameState->SetLife(CurrentLife);
        PS_GameState->SetGameStart(bIsGameStart);
    }
}

void APS_GameMode::InitVariables()
{
    CurrentMap = 1;
    CurrentStage = 1;
    CurrentLife = 3;
    bIsGameStart = false;

    if (UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance()))
    {
        PS_GameInstance->SetMap(CurrentMap);
        PS_GameInstance->SetStage(CurrentStage);
        PS_GameInstance->SetLife(CurrentLife);
        PS_GameInstance->SetIsGameStart(bIsGameStart);
    }
    if (APS_GameState* PS_GameState = GetGameState<APS_GameState>())
    {
        PS_GameState->SetStage(CurrentMap, CurrentStage);
        PS_GameState->SetLife(CurrentLife);
        PS_GameState->SetGameStart(bIsGameStart);
    }
}

void APS_GameMode::OnHUDInitialized()
{
    CurrentPlayersCount++;
    UE_LOG(Project_S, Log, TEXT("CurrentPlayersCount = %d\n"), CurrentPlayersCount);

    if (bIsGameStart)
    {
        UE_LOG(Project_S, Log, TEXT("bIsGameStart is true\n"));
    }
    else
    {
        UE_LOG(Project_S, Log, TEXT("bIsGameStart is false\n"));
    }

    // 모든 Player의 Stage UI 수정
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
    {
        APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
        if (PS_PlayerController)
        {
            PS_PlayerController->HideStageUI();
            PS_PlayerController->ShowStageTextUI(FString(TEXT("시작 대기중입니다")));
        }
    }

    if (bIsGameStart && CurrentPlayersCount == 2)
    {
        //StartFirstWordSelectionTimer(SelectionTime);
        PostStartFirstWordSelectionTimer(SelectionTime);
    }
}

void APS_GameMode::StartGameAfter5Seconds()
{
    // 타이머 설정
    GetWorldTimerManager().SetTimer(StartGameTimerHandle, this, &APS_GameMode::OnStartGameAfter5SecondsComplete, GameStartWaitTime, false);

    // 모든 Player의 Stage UI 수정
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
    {
        APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
        if (PS_PlayerController)
        {
            PS_PlayerController->ShowStageTextUI(FString(TEXT("잠시 후 게임 스테이지로 이동합니다")));
            PS_PlayerController->ReadyStartGame(GetWorld()->GetTimerManager().GetTimerRemaining(StartGameTimerHandle));
        }
    }
}

void APS_GameMode::ClearStartGameTimer()
{
    GetWorldTimerManager().ClearTimer(StartGameTimerHandle);
    //StartGameTimerHandle.Invalidate();

    // 모든 Player의 Stage UI 수정
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
    {
        APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
        if (PS_PlayerController)
        {
            PS_PlayerController->ShowStageTextUI(FString(TEXT("시작 대기중입니다")));
            PS_PlayerController->CancelStartGame();
        }
    }
}

void APS_GameMode::OnStartGameAfter5SecondsComplete()
{
    PS_LOG_S(Log);

    bIsGameStart = true;
    if (UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance()))
    {
        //if (!PS_GameInstance->StartGame()) return;

        PS_GameInstance->SetIsGameStart(bIsGameStart);
    }
    if (APS_GameState* PS_GameState = GetGameState<APS_GameState>())
    {
        PS_GameState->SetGameStart(bIsGameStart);
    }

    // 모든 Player의 Stage UI 수정
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
    {
        APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
        if (PS_PlayerController)
        {
            PS_PlayerController->HideStageTextUI();
            PS_PlayerController->HideStageTimerUI();
        }
    }

    TransitionToStage(1, 1);
}

void APS_GameMode::PostStartFirstWordSelectionTimer(int TimeLimit)
{
    // 타이머 설정
    GetWorldTimerManager().SetTimer(SelectionUITimerHandle, this, &APS_GameMode::OnPostStartFirstWordSelectionTimerComplete, TimeLimit, false);

    // 모든 Player에 대기 메시지 추가
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
    {
        APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
        if (PS_PlayerController)
        {
            if (CurrentStage == 1)
            {
                PS_PlayerController->ShowStageTextUI(FString(TEXT("잠시 후 게임이 시작됩니다")));
            }
            else
            {
                PS_PlayerController->ShowStageTextUI(FString(TEXT("플레이어 전환 대기 중입니다")));
            }
        }
    }
}

void APS_GameMode::OnPostStartFirstWordSelectionTimerComplete()
{
    // 모든 Player에 대기 메시지 제거
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
    {
        APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
        if (PS_PlayerController)
        {
            PS_PlayerController->HideStageTextUI();
        }
    }

    StartFirstWordSelectionTimer(SelectionTime);
}

void APS_GameMode::StartFirstWordSelectionTimer(int TimeLimit)
{
    PS_LOG_S(Log);

    ButtonWords = InitializeWords(CurrentMap, CurrentStage, 3);

    // 모든 Player에 Stage UI 추가
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
    {
        APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
        if (PS_PlayerController)
        {
            PS_PlayerController->ShowStageUI();
        }
    }

    // 타이머 설정
    GetWorldTimerManager().SetTimer(SelectionUITimerHandle, this, &APS_GameMode::OnFirstWordSelectionComplete, TimeLimit, false);

    // 첫번째 PlayerController에 단어 선택 UI 추가
    FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();
    APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        APS_PlayerState* PS_PlayerState = Cast<APS_PlayerState>(PS_PlayerController->PlayerState);
        if (PS_PlayerState)
        {
            PS_PlayerState->InitSelectedWord();
        }
        PS_PlayerController->ShowWordSelectionUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle));
        PS_PlayerController->SetSelectionButtonWords(ButtonWords);
    }

    // 두번째 PlayerController에 대기 UI 추가
    It++;
    PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowStageTextUI(FString(TEXT("")));
        PS_PlayerController->ShowWordSelectionWaitUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle));
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
        PS_PlayerController->ShowWordSelectionUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle));
    }

    // 첫번째 PlayerController의 bHasSelectedWord가 true인지 검사
    APS_PlayerState* PS_PlayerState = Cast<APS_PlayerState>(PS_PlayerController->PlayerState);
    if (PS_PlayerState)
    {
        // 선택하지 않았으면 랜덤으로 선택
        if (!PS_PlayerState->GetHasSelectedWord())
        {
            UE_LOG(Project_S, Log, TEXT("Randomly Selected..\n"));
            int32 RandomIndex = FMath::RandRange(0, ButtonWords.Num() - 1);
            PS_PlayerState->UpdateSelectedWord_Server(ButtonWords[RandomIndex]);
        }

        // 사용된 제시어 목록에 추가
        UsedWords.Add(PS_PlayerState->GetSelectedWord());
        Answer = PS_PlayerState->GetSelectedWord();
    }

    // 두번째 PlayerController에 대기 UI 제거
    It++;
    PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowWordSelectionWaitUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle));
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

    // 첫번째 PlayerController에 선택한 단어 노출
    APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(GetWorld()->GetPlayerControllerIterator()->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowStageWordUI(Answer);

        APS_PlayerState* PS_PlayerState = Cast<APS_PlayerState>(PS_PlayerController->PlayerState);
        if (PS_PlayerState)
        {
            UE_LOG(Project_S, Log, TEXT("Owner : %s, Selected Word : %s\n"), *PS_PlayerController->GetName(), *PS_PlayerState->GetSelectedWord());
        }
    }

    
    // 타이머 설정
    GetWorldTimerManager().SetTimer(GameSessionTimerHandle, this, &APS_GameMode::OnFirstGameSessionEnd, TimeLimit, false);

    // 모든 Player에 Timer UI 추가
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
    {
        PS_PlayerController = Cast<APS_PlayerController>(It->Get());
        if (PS_PlayerController)
        {
            PS_PlayerController->ShowStageTimerUI(GetWorld()->GetTimerManager().GetTimerRemaining(GameSessionTimerHandle));
        }
    }
}

void APS_GameMode::OnFirstGameSessionEnd()
{
    PS_LOG_S(Log);

    // 첫번째 PlayerController에 선택한 단어 제거
    APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(GetWorld()->GetPlayerControllerIterator()->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->HideStageWordUI();
    }

    // 모든 Player에 Timer UI 제거
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
    {
        PS_PlayerController = Cast<APS_PlayerController>(It->Get());
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
        PS_PlayerController->ShowAnswerSelectionWaitUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle));
    }

    // 두번째 PlayerController에 정답 선택 UI 추가
    It++;
    PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        APS_PlayerState* PS_PlayerState = Cast<APS_PlayerState>(PS_PlayerController->PlayerState);
        if (PS_PlayerState)
        {
            PS_PlayerState->InitSelectedWord();
        }
        PS_PlayerController->ShowAnswerSelectionUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle));

        TArray<FString> AnswerLists = InitializeWords(CurrentMap, CurrentStage, 2);
        PS_PlayerState = Cast<APS_PlayerState>((It - 1)->Get()->PlayerState);
        if (PS_PlayerState)
        {
            AnswerLists.Add(PS_PlayerState->GetSelectedWord());

            // 랜덤으로 섞음
            Algo::RandomShuffle(AnswerLists);
        }

        PS_PlayerController->SetAnswerSelectionButtonWords(AnswerLists);
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
        PS_PlayerController->ShowAnswerSelectionWaitUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle));
    }

    // 두번째 PlayerController에 정답 선택 UI 제거
    It++;
    PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowAnswerSelectionUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle));
    }

    FirstAnswerShow(SelectionTime);
}

void APS_GameMode::FirstAnswerShow(int TimeLimit)
{
    // 타이머 설정
    GetWorldTimerManager().SetTimer(SelectionUITimerHandle, this, &APS_GameMode::OnFirstAnswerShowComplete, TimeLimit, false);
    float RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle);

    FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();
    It++;
    APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    APS_PlayerState* PS_PlayerState = Cast<APS_PlayerState>(PS_PlayerController->PlayerState);
    if (PS_PlayerState)
    {
        if (PS_PlayerState->GetHasSelectedWord())
        {
            // 정답
            if (UsedWords.Last().Equals(PS_PlayerState->GetSelectedWord()))
            {
                UE_LOG(Project_S, Log, TEXT("Correct!\n"));

                bIsCorrect = true;
                SelectedWord = PS_PlayerState->GetSelectedWord();
                // 모든 Player에 Wrong UI 추가
                for (FConstPlayerControllerIterator It_ = GetWorld()->GetPlayerControllerIterator(); It_; It_++)
                {
                    PS_PlayerController = Cast<APS_PlayerController>(It_->Get());
                    if (PS_PlayerController)
                    {
                        PS_PlayerController->ToggleCorrectUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle), Answer, SelectedWord);
                    }
                }
            }
            // 오답
            else
            {
                UE_LOG(Project_S, Log, TEXT("Answer : %s\n"), *PS_PlayerState->GetSelectedWord());
                UE_LOG(Project_S, Log, TEXT("Wrong!\n"));
                bIsCorrect = false;
                SelectedWord = PS_PlayerState->GetSelectedWord();
                if (UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance()))
                {
                    PS_GameInstance->DeductLife();

                    APS_GameState* PS_GameState = GetGameState<APS_GameState>();
                    if (PS_GameState)
                    {
                        PS_GameState->SetLife(--CurrentLife);

                        // 모든 Player에 Wrong UI 추가
                        for (FConstPlayerControllerIterator It_ = GetWorld()->GetPlayerControllerIterator(); It_; It_++)
                        {
                            PS_PlayerController = Cast<APS_PlayerController>(It_->Get());
                            if (PS_PlayerController)
                            {
                                PS_PlayerController->ToggleWrongUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle), Answer, SelectedWord);
                            }
                        }
                    }
                }
            }
        }
        // 오답
        else
        {
            UE_LOG(Project_S, Log, TEXT("Answer is not selected!\n"));
            UE_LOG(Project_S, Log, TEXT("Wrong!\n"));
            bIsCorrect = false;
            SelectedWord = PS_PlayerState->GetSelectedWord();
            if (UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance()))
            {
                PS_GameInstance->DeductLife();

                APS_GameState* PS_GameState = GetGameState<APS_GameState>();
                if (PS_GameState)
                {
                    PS_GameState->SetLife(--CurrentLife);

                    // 모든 Player에 Wrong UI 추가
                    for (FConstPlayerControllerIterator It_ = GetWorld()->GetPlayerControllerIterator(); It_; It_++)
                    {
                        PS_PlayerController = Cast<APS_PlayerController>(It_->Get());
                        if (PS_PlayerController)
                        {
                            PS_PlayerController->ToggleWrongUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle), Answer, SelectedWord);
                        }
                    }
                }
            }
        }
    }
}

void APS_GameMode::OnFirstAnswerShowComplete()
{
    if (bIsCorrect)
    {
        // 모든 Player에 Correct UI 제거
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
        {
            APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
            if (PS_PlayerController)
            {
                PS_PlayerController->ToggleCorrectUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle), Answer, SelectedWord);
            }
        }
    }
    else
    {
        // 모든 Player에 Wrong UI 제거
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
        {
            APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
            if (PS_PlayerController)
            {
                PS_PlayerController->ToggleWrongUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle), Answer, SelectedWord);
            }
        }
    }

    if (CurrentLife > 0)
    {
        PostStartSecondWordSelectionTimer(SelectionTime);
    }
    else
    {
        // 게임 오버
        bIsGameStart = false;
        if (UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance()))
        {
            PS_GameInstance->SetIsGameStart(bIsGameStart);
        }
        if (APS_GameState* PS_GameState = GetGameState<APS_GameState>())
        {
            PS_GameState->SetGameStart(bIsGameStart);
        }

        // 모든 Player에 게임 오버 추가
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
        {
            APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
            if (PS_PlayerController)
            {
                PS_PlayerController->ShowStageTextUI(FString(TEXT("게임 오버")));
                if (It == GetWorld()->GetPlayerControllerIterator())
                {
                    PS_PlayerController->ShowGameOverUI(true);
                }
                else
                {
                    PS_PlayerController->ShowGameOverUI(false);
                }
            }
        }
    }
}

void APS_GameMode::PostStartSecondWordSelectionTimer(int TimeLimit)
{
    // 타이머 설정
    GetWorldTimerManager().SetTimer(SelectionUITimerHandle, this, &APS_GameMode::OnPostStartSecondWordSelectionTimerComplete, TimeLimit, false);

    // 모든 Player에 대기 메시지 추가
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
    {
        APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
        if (PS_PlayerController)
        {
            PS_PlayerController->ShowStageTextUI(FString(TEXT("플레이어 전환 대기 중입니다")));
        }
    }
}

void APS_GameMode::OnPostStartSecondWordSelectionTimerComplete()
{
    // 모든 Player에 대기 메시지 제거
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
    {
        APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
        if (PS_PlayerController)
        {
            PS_PlayerController->ShowStageTextUI(FString(TEXT("")));
        }
    }

    StartSecondWordSelectionTimer(SelectionTime);
}

void APS_GameMode::StartSecondWordSelectionTimer(int TimeLimit)
{
    PS_LOG_S(Log);

    ButtonWords = InitializeWords(CurrentMap, CurrentStage, 3);

    // 타이머 설정
    GetWorldTimerManager().SetTimer(SelectionUITimerHandle, this, &APS_GameMode::OnSecondWordSelectionComplete, TimeLimit, false);

    // 첫번째 PlayerController에 대기 UI 추가
    FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();
    APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowWordSelectionWaitUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle));
    }

    // 두번째 PlayerController에 단어 선택 UI 추가
    It++;
    PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        APS_PlayerState* PS_PlayerState = Cast<APS_PlayerState>(PS_PlayerController->PlayerState);
        if (PS_PlayerState)
        {
            PS_PlayerState->InitSelectedWord();
        }
        PS_PlayerController->ShowWordSelectionUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle));
        PS_PlayerController->SetSelectionButtonWords(ButtonWords);
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
        PS_PlayerController->ShowWordSelectionWaitUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle));
    }

    // 두번째 PlayerController에 단어 선택 UI 제거
    It++;
    PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowWordSelectionUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle));
    }

    // 두번째 PlayerController의 bHasSelectedWord가 true인지 검사
    APS_PlayerState* PS_PlayerState = Cast<APS_PlayerState>(PS_PlayerController->PlayerState);
    if (PS_PlayerState)
    {
        // 선택하지 않았으면 랜덤으로 선택
        if (!PS_PlayerState->GetHasSelectedWord())
        {
            int32 RandomIndex = FMath::RandRange(0, ButtonWords.Num() - 1);
            PS_PlayerState->UpdateSelectedWord_Server(ButtonWords[RandomIndex]);
        }

        // 사용된 제시어 목록에 추가
        UsedWords.Add(PS_PlayerState->GetSelectedWord());
        Answer = PS_PlayerState->GetSelectedWord();
    }

    StartSecondGameSession(GameSessionTime);
}

void APS_GameMode::StartSecondGameSession(int TimeLimit)
{
    PS_LOG_S(Log);

    // 두번째 PlayerController에 선택한 단어 노출
    FConstPlayerControllerIterator It_ = GetWorld()->GetPlayerControllerIterator();
    It_++;
    APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It_->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowStageWordUI(Answer);
    }

    // 타이머 설정
    GetWorldTimerManager().SetTimer(GameSessionTimerHandle, this, &APS_GameMode::OnSecondGameSessionEnd, TimeLimit, false);

    // 모든 Player에 Timer UI 추가
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
    {
        PS_PlayerController = Cast<APS_PlayerController>(It->Get());
        if (PS_PlayerController)
        {
            PS_PlayerController->ShowStageTimerUI(GetWorld()->GetTimerManager().GetTimerRemaining(GameSessionTimerHandle));
        }
    }
}

void APS_GameMode::OnSecondGameSessionEnd()
{
    PS_LOG_S(Log);
    // 두번째 PlayerController에 선택한 단어 제거
    FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();
    It++;
    APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->HideStageWordUI();
    }

    // 모든 Player에 Timer UI 제거
    for (FConstPlayerControllerIterator It_ = GetWorld()->GetPlayerControllerIterator(); It_; It_++)
    {
        PS_PlayerController = Cast<APS_PlayerController>(It_->Get());
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
        APS_PlayerState* PS_PlayerState = Cast<APS_PlayerState>(PS_PlayerController->PlayerState);
        if (PS_PlayerState)
        {
            PS_PlayerState->InitSelectedWord();
        }
        PS_PlayerController->ShowAnswerSelectionUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle));

        TArray<FString> AnswerLists = InitializeWords(CurrentMap, CurrentStage, 2);
        PS_PlayerState = Cast<APS_PlayerState>((It + 1)->Get()->PlayerState);
        if (PS_PlayerState)
        {
            AnswerLists.Add(PS_PlayerState->GetSelectedWord());

            // 랜덤으로 섞음
            Algo::RandomShuffle(AnswerLists);
        }

        PS_PlayerController->SetAnswerSelectionButtonWords(AnswerLists);
    }

    // 두번째 PlayerController에 대기 UI 추가
    It++;
    PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowAnswerSelectionWaitUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle));
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
        PS_PlayerController->ShowAnswerSelectionUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle));
    }

    // 두번째 PlayerController에 대기 UI 제거
    It++;
    PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    if (PS_PlayerController)
    {
        PS_PlayerController->ShowAnswerSelectionWaitUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle));
    }

    SecondAnswerShow(SelectionTime);
}

void APS_GameMode::SecondAnswerShow(int TimeLimit)
{
    // 타이머 설정
    GetWorldTimerManager().SetTimer(SelectionUITimerHandle, this, &APS_GameMode::OnSecondAnswerShowComplete, TimeLimit, false);

    // 첫번째 PlayerController의 bHasSelectedWord가 true인지 검사
    FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();
    APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
    APS_PlayerState* PS_PlayerState = Cast<APS_PlayerState>(PS_PlayerController->PlayerState);
    if (PS_PlayerState)
    {
        // 정답
        if (PS_PlayerState->GetHasSelectedWord())
        {
            // 정답
            if (UsedWords.Last().Equals(PS_PlayerState->GetSelectedWord()))
            {
                UE_LOG(Project_S, Log, TEXT("Correct!\n"));
                bIsCorrect = true;
                SelectedWord = PS_PlayerState->GetSelectedWord();
                // 모든 Player에 Correct UI 추가
                for (FConstPlayerControllerIterator It_ = GetWorld()->GetPlayerControllerIterator(); It_; It_++)
                {
                    PS_PlayerController = Cast<APS_PlayerController>(It_->Get());
                    if (PS_PlayerController)
                    {
                        PS_PlayerController->ToggleCorrectUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle), Answer, SelectedWord);
                    }
                }
            }
            // 오답
            else
            {
                UE_LOG(Project_S, Log, TEXT("Answer : %s\n"), *PS_PlayerState->GetSelectedWord());
                UE_LOG(Project_S, Log, TEXT("Wrong!\n"));
                bIsCorrect = false;
                SelectedWord = PS_PlayerState->GetSelectedWord();
                if (UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance()))
                {
                    PS_GameInstance->DeductLife();

                    APS_GameState* PS_GameState = GetGameState<APS_GameState>();
                    if (PS_GameState)
                    {
                        PS_GameState->SetLife(--CurrentLife);

                        // 모든 Player에 Wrong UI 추가
                        for (FConstPlayerControllerIterator It_ = GetWorld()->GetPlayerControllerIterator(); It_; It_++)
                        {
                            PS_PlayerController = Cast<APS_PlayerController>(It_->Get());
                            if (PS_PlayerController)
                            {
                                PS_PlayerController->ToggleWrongUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle), Answer, SelectedWord);
                            }
                        }
                    }
                }
            }
        }
        // 오답
        else
        {
            UE_LOG(Project_S, Log, TEXT("Answer is not selected!\n"));
            UE_LOG(Project_S, Log, TEXT("Wrong!\n"));
            bIsCorrect = false;
            SelectedWord = PS_PlayerState->GetSelectedWord();
            if (UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance()))
            {
                PS_GameInstance->DeductLife();

                APS_GameState* PS_GameState = GetGameState<APS_GameState>();
                if (PS_GameState)
                {
                    PS_GameState->SetLife(--CurrentLife);

                    // 모든 Player에 Wrong UI 추가
                    for (FConstPlayerControllerIterator It_ = GetWorld()->GetPlayerControllerIterator(); It_; It_++)
                    {
                        PS_PlayerController = Cast<APS_PlayerController>(It_->Get());
                        if (PS_PlayerController)
                        {
                            PS_PlayerController->ToggleWrongUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle), Answer, SelectedWord);
                        }
                    }
                }
            }
        }
    }
}

void APS_GameMode::OnSecondAnswerShowComplete()
{
    if (bIsCorrect)
    {
        // 모든 Player에 Correct UI 제거
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
        {
            APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
            if (PS_PlayerController)
            {
                PS_PlayerController->ToggleCorrectUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle), Answer, SelectedWord);
            }
        }
    }
    else
    {
        // 모든 Player에 Wrong UI 제거
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
        {
            APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
            if (PS_PlayerController)
            {
                PS_PlayerController->ToggleWrongUI(GetWorld()->GetTimerManager().GetTimerRemaining(SelectionUITimerHandle), Answer, SelectedWord);
            }
        }
    }

    if (CurrentLife > 0)
    {
        CurrentStage++;
        if (CurrentStage > 10)
        {
            CurrentMap++;
            CurrentStage -= 10;

            if (UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance()))
            {
                PS_GameInstance->SetMap(CurrentMap);
                PS_GameInstance->SetStage(CurrentStage);
                PS_GameInstance->SetLife(CurrentLife);

                APS_GameState* PS_GameState = GetGameState<APS_GameState>();
                if (PS_GameState)
                {
                    PS_GameState->SetStage(CurrentMap, CurrentStage);
                    PS_GameState->SetLife(CurrentLife);
                }

                TransitionToStage(CurrentMap, CurrentStage);
            }
        }
        else
        {
            if (UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance()))
            {
                PS_GameInstance->SetMap(CurrentMap);
                PS_GameInstance->SetStage(CurrentStage);
                PS_GameInstance->SetLife(CurrentLife);

                APS_GameState* PS_GameState = GetGameState<APS_GameState>();
                if (PS_GameState)
                {
                    PS_GameState->SetStage(CurrentMap, CurrentStage);
                    PS_GameState->SetLife(CurrentLife);
                }
            }

            PostStartFirstWordSelectionTimer(SelectionTime);
        }
    }
    else
    {
        // 게임 오버
        bIsGameStart = false;
        if (UPS_GameInstance* PS_GameInstance = Cast<UPS_GameInstance>(GetGameInstance()))
        {
            PS_GameInstance->SetIsGameStart(bIsGameStart);
        }
        if (APS_GameState* PS_GameState = GetGameState<APS_GameState>())
        {
            PS_GameState->SetGameStart(bIsGameStart);
        }

        // 모든 Player에 게임 오버 추가
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
        {
            APS_PlayerController* PS_PlayerController = Cast<APS_PlayerController>(It->Get());
            if (PS_PlayerController)
            {
                PS_PlayerController->ShowStageTextUI(FString(TEXT("게임 오버")));
                if (It == GetWorld()->GetPlayerControllerIterator())
                {
                    PS_PlayerController->ShowGameOverUI(true);
                }
                else
                {
                    PS_PlayerController->ShowGameOverUI(false);
                }
            }
        }
    }
}