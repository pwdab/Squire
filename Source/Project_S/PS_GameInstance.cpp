// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_GameInstance.h"

UPS_GameInstance::UPS_GameInstance()
{
    Map = 1;
    Stage = 1;
    Life = 3;
}

void UPS_GameInstance::Init()
{
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        SessionInterface = OnlineSubsystem->GetSessionInterface();
        if (UWorld* World = GetWorld())
        {
            if (UGameInstance* GameInstance = World->GetGameInstance())
            {
                UE_LOG(Project_S, Log, TEXT("Online : %s, GameInstance : %s"), *OnlineSubsystem->GetSubsystemName().ToString(), *GameInstance->GetName());

                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Online : %s, GameInstance : %s"), *OnlineSubsystem->GetSubsystemName().ToString(), *GameInstance->GetName()));
                }
            }
        }

        if (SessionInterface.IsValid())
        {
            // Setup Delegates
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPS_GameInstance::OnCreateSessionComplete);
            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPS_GameInstance::OnDestroySessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPS_GameInstance::OnFindSessionsComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPS_GameInstance::OnJoinSessionComplete);

            UE_LOG(Project_S, Log, TEXT("GameInstance : %s, Setup Delegates Complete"), *this->GetName());
        }
        else
        {
            PS_LOG_S(Log);
            UE_LOG(Project_S, Warning, TEXT("SessionInterface is Not Valid!!"));
        }
    }
}

void UPS_GameInstance::CreateSession()
{
    PS_LOG_S(Log);
    UE_LOG(Project_S, Log, TEXT("Create Session"));
    if (SessionInterface.IsValid())
    {
        // Set the Handle
        SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSesionCompleteDelegate);

        // 기존 Session 제거
        const auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession); // 
        if (ExistingSession != nullptr)
        {
            SessionInterface->DestroySession(NAME_GameSession);
        }

        TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
        SessionSettings->bIsLANMatch = false;			// Lan을 통한 연결
        SessionSettings->NumPublicConnections = 2;		// 연결 가능 인원
        SessionSettings->bAllowJoinInProgress = true;	// 세션이 실행 중 참여가능 여부
        SessionSettings->bAllowJoinViaPresence = true;	// ???
        SessionSettings->bShouldAdvertise = true;		// Steam을 통해 세션을 알림(세션 조회 가능)
        SessionSettings->bUsesPresence = true;			// ???
        SessionSettings->bUseLobbiesIfAvailable = true; // Lobby 사용 여부
        SessionSettings->Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing); // 세션의 MatchType을 모두에게 열림, 온라인서비스와 핑을 통해 세션 홍보 옵션으로 설정

        // GameInstance에서 실행하기 때문(PlayerController를 얻어올 수 있거나 갖고 있다면 써도됨)
        const ULocalPlayer* Localplayer = GetWorld()->GetFirstLocalPlayerFromController();
        SessionInterface->CreateSession(*Localplayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings);
        UE_LOG(Project_S, Log, TEXT("Create Session Complete"));
    }
    else
    {
        UE_LOG(Project_S, Log, TEXT("Create Session Error"));
    }
}

void UPS_GameInstance::DestroySession()
{
    if (SessionInterface.IsValid())
    {
        // Set the Handle
        SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
    }
}

void UPS_GameInstance::FindSession()
{
    UE_LOG(Project_S, Log, TEXT("Find Session Start"));

    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (!LocalPlayer || !LocalPlayer->GetPreferredUniqueNetId().IsValid())
    {
        UE_LOG(Project_S, Error, TEXT("LocalPlayer or UniqueNetId is not valid."));
        return;
    }

    if (SessionInterface.IsValid())
    {
        // Find Session Complete Delegate 등록
        SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

        // Find Game Session
        SessionSearch = MakeShareable(new FOnlineSessionSearch());
        UE_LOG(Project_S, Log, TEXT("SessionSearch created: %p"), SessionSearch.Get());
        SessionSearch->MaxSearchResults = 10;	// 검색 결과로 나오는 세션 수 최대값
        SessionSearch->bIsLanQuery = false;			// LAN 사용 여부
        SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals); // 찾을 세션 쿼리를 현재로 설정한다

        //const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
        SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
        UE_LOG(Project_S, Log, TEXT("Find Session Complete"));
    }
    else
    {
        UE_LOG(Project_S, Log, TEXT("Find Session Error"));
    }
}

void UPS_GameInstance::JoinSession()
{
    if (SessionInterface.IsValid())
    {
        // Set the Handle
        SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

        // idx: 원하는 Session의 index
        int idx = 0;
        const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
        auto SessionName = FName(SessionSearch->SearchResults[idx].Session.OwningUserName);
        auto Result = SessionSearch->SearchResults[idx];
        SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), SessionName, Result);
    }
}

void UPS_GameInstance::OnCreateSessionComplete(FName SesionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        // 세션 생성 성공
    }
    else
    {
        // 세션 생성 실패
    }
}

void UPS_GameInstance::OnDestroySessionComplete(FName SesionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        // 세션 생성 성공
    }
    else
    {
        // 세션 생성 실패
    }
}

void UPS_GameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(Project_S, Log, TEXT("GameInstance : %s, Find Sessions Complete Successfully"), *this->GetName());
        UE_LOG(Project_S, Log, TEXT("OnFindSessionsComplete called. SessionSearch pointer: %p"), SessionSearch.Get());
        // 성공
        if (SessionSearch.IsValid())
        {
            TArray<FBlueprintSessionResult> BlueprintSessionResults;
            for (const auto& Result : SessionSearch->SearchResults)
            {
                if (Result.IsValid())
                {
                    FBlueprintSessionResult BlueprintSessionResult;
                    BlueprintSessionResult.OnlineResult = Result;
                    BlueprintSessionResults.Add(BlueprintSessionResult);
                }
                else
                {
                    UE_LOG(Project_S, Warning, TEXT("SearchResult not Valid"));
                }
            }

            BlueprintFindSessionsCompleteDelegate.Broadcast(BlueprintSessionResults);
        }
        else
        {
            UE_LOG(Project_S, Log, TEXT("SessionSearch is not Valid"));
        }
    }
    else
    {
        // 세션 생성 실패
        UE_LOG(Project_S, Log, TEXT("Find Sessions Complete Failure"));
    }
}

void UPS_GameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (SessionInterface.IsValid())
    {
        APlayerController* const PlayerController = GetFirstLocalPlayerController();
        FString TravelURL;

        if (PlayerController && SessionInterface->GetResolvedConnectString(SessionName, TravelURL))
        {
            PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
        }
    }
}

void UPS_GameInstance::SetMap(uint8 InMap)
{
    Map = InMap;
}

void UPS_GameInstance::SetStage(uint8 InStage)
{
    Stage = InStage;
}

void UPS_GameInstance::SetLife(uint8 InLife)
{
    Life = InLife;
}

void UPS_GameInstance::DeductLife()
{
    Life--;
}

uint8 UPS_GameInstance::GetMap() const
{
    return Map;
}

uint8 UPS_GameInstance::GetStage() const
{
    return Stage;
}

uint8 UPS_GameInstance::GetLife() const
{
    return Life;
}

bool UPS_GameInstance::IsGameStart() const
{
    return bIsGameStart;
}

void UPS_GameInstance::SetIsGameStart(bool NewBool)
{
    PS_LOG_S(Log);
    bIsGameStart = NewBool;

    if (bIsGameStart)
    {
        UE_LOG(Project_S, Log, TEXT("bIsGameStart is true\n"));
    }
    else
    {
        UE_LOG(Project_S, Log, TEXT("bIsGameStart is false\n"));
    }
}