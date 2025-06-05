// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_GameInstance.h"

DEFINE_LOG_CATEGORY(LogPSGameInstance);

UPS_GameInstance::UPS_GameInstance()
    : PendingSessionPassword(TEXT(""))
    , JoinAttemptPassword(TEXT(""))
    , bIsPrivateSession(false)
    , CurrentSessionName(NAME_None)
    , bIsProcessingSession(false)
{
    Map = 1;
    Stage = 1;
    Life = 3;
}

void UPS_GameInstance::Init()
{
    Super::Init();

    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        SessionInterface = OnlineSubsystem->GetSessionInterface();

        // 디버그용
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
            /*
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPS_GameInstance::OnCreateSessionComplete);
            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPS_GameInstance::OnDestroySessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPS_GameInstance::OnFindSessionsComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPS_GameInstance::OnJoinSessionComplete);
            */

            // CreateSession 델리게이트 바인딩
            OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(
                this, &UPS_GameInstance::OnCreateSessionComplete);
            SessionInterface->OnCreateSessionCompleteDelegates.Add(OnCreateSessionCompleteDelegate);

            // EndSession 델리게이트 바인딩
            OnEndSessionCompleteDelegate = FOnEndSessionCompleteDelegate::CreateUObject(
                this, &UPS_GameInstance::OnEndSessionComplete
            );

            // DestroySession 델리게이트 바인딩
            OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(
                this, &UPS_GameInstance::OnDestroySessionComplete);
            SessionInterface->OnDestroySessionCompleteDelegates.Add(OnDestroySessionCompleteDelegate);

            // FindSessions 델리게이트 바인딩
            OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(
                this, &UPS_GameInstance::OnFindSessionsComplete);
            // 바인딩은 FindSessions 호출 시 Handle을 저장하고 사용

            // JoinSession 델리게이트 바인딩
            OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(
                this, &UPS_GameInstance::OnJoinSessionComplete);
            // 이벤트 바인딩은 JoinSession 호출 시 Handle을 저장하고 사용

            // SessionParticipantRemoved 델리게이트 바인딩 (호스트가 참가자 퇴장을 감지하기 위함)
            OnSessionParticipantRemovedDelegate = FOnSessionParticipantRemovedDelegate::CreateUObject(
                this, &UPS_GameInstance::OnSessionParticipantRemoved);
            OnSessionParticipantRemovedDelegateHandle = SessionInterface->AddOnSessionParticipantRemovedDelegate_Handle(OnSessionParticipantRemovedDelegate);

            UE_LOG(LogPSGameInstance, Warning, TEXT("GameInstance : %s, Setup Delegates Complete"), *this->GetName());
        }
        else
        {
            UE_LOG(LogPSGameInstance, Warning, TEXT("SessionInterface is not valid!"));
        }
    }
    else
    {
        UE_LOG(LogPSGameInstance, Warning, TEXT("OnlineSubsystem not found!"));
    }

    //bIsJoining = false;
}

/*
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
            UE_LOG(Project_S, Log, TEXT("There is Existing Session.. Destroying.."));
            SessionInterface->DestroySession(NAME_GameSession);
        }
        else
        {
            UE_LOG(Project_S, Log, TEXT("There is no Existing Session.."));
        }

        TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
        SessionSettings->bIsLANMatch = false;			// Lan을 통한 연결
        SessionSettings->NumPublicConnections = 2;		// 연결 가능 인원
        SessionSettings->bAllowJoinInProgress = false;	// 세션이 실행 중 참여가능 여부
        SessionSettings->bAllowJoinViaPresence = true;	// ???
        SessionSettings->bShouldAdvertise = true;		// Steam을 통해 세션을 알림(세션 조회 가능)
        SessionSettings->bUsesPresence = true;			// ???
        //SessionSettings->bAllowInvites = true;
        //SessionSettings->bAllowJoinViaPresence = true;
        //SessionSettings->bAllowJoinViaPresenceFriendsOnly = true;
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

void UPS_GameInstance::OnCreateSessionComplete(FName SesionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        // 세션 생성 성공
        UE_LOG(Project_S, Log, TEXT("Create Session Complete Successfully"));
        bool IsSuccessful = SessionInterface->StartSession(NAME_GameSession);

        if (IsSuccessful)
        {
            UE_LOG(Project_S, Log, TEXT("Start Session Complete"));
            //ClientTravel(TEXT("/Game/Maps/Level_MainMenu"), ETravelType::TRAVEL_Absolute);
        }
        else
        {
            UE_LOG(Project_S, Log, TEXT("Start Session Fail"));
        }
    }
    else
    {
        // 세션 생성 실패
    }
}
*/

#pragma region CreateSession

void UPS_GameInstance::CreateSession(bool bMakePrivate, const FString& InPassword)
{
    if (!SessionInterface.IsValid())
    {
        UE_LOG(LogPSGameInstance, Error, TEXT("Cannot Create Session: SessionInterface invalid"));
        BlueprintJoinSessionsCompleteDelegate.Broadcast(false, TEXT("세션 시스템 오류"));
        return;
    }

    if (bIsProcessingSession)
    {
        UE_LOG(LogPSGameInstance, Warning, TEXT("이미 다른 세션 작업 진행 중입니다."));
        return;
    }

    // 이미 생성된 세션이 있다면 파괴
    if (SessionInterface->GetNamedSession(CurrentSessionName) != nullptr)
    {
        UE_LOG(LogPSGameInstance, Log, TEXT("기존 세션이 존재하여 Destroy 후 새로 생성"));
        SessionInterface->DestroySession(CurrentSessionName);
    }

    /*
    // 호스트 닉네임을 세션 이름으로 저장 (옵션)
    IOnlineIdentityPtr Identity = IOnlineSubsystem::Get()->GetIdentityInterface();
    if (Identity.IsValid() && Identity->GetUniquePlayerId(0).IsValid())
    {
        FString UserName = Identity->GetPlayerNickname(0);
        SessionSettings->Set(FName("HostName"), UserName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
    }
    */

    // 새로 생성할 세션 이름 지정
    // 호스트 닉네임과 UniqueNetId 문자열을 조합하여 세션 이름 생성
    FString HostNick = TEXT("Host");
    FString UniqueIdStr = TEXT("UnknownID");
    IOnlineIdentityPtr Identity = IOnlineSubsystem::Get()->GetIdentityInterface();
    if (Identity.IsValid())
    {
        TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(0);
        if (UserId.IsValid())
        {
            HostNick = Identity->GetPlayerNickname(*UserId);
            UniqueIdStr = UserId->ToString(); // 예: SteamID 문자열
        }
    }

    // 최종 세션 이름: "HostNick_UniqueIdStr"
    FString SessionNameString = FString::Printf(TEXT("%s_%s"), *HostNick, *UniqueIdStr);
    CurrentSessionName = FName(*SessionNameString);

    // 세션 설정
    TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
    SessionSettings->bIsLANMatch = false; 
    SessionSettings->NumPublicConnections = MAX_PUBLIC_CONNECTIONS;
    SessionSettings->bAllowJoinInProgress = true;
    SessionSettings->bAllowJoinViaPresence = true;
    SessionSettings->bShouldAdvertise = true;
    SessionSettings->bUsesPresence = true;
    SessionSettings->bUseLobbiesIfAvailable = true;
    SessionSettings->bIsDedicated = false;
    SessionSettings->Set(FName("HostName"), HostNick, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
    SessionSettings->Set(FName("CurrentSessionName"), SessionNameString, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

    FString DebugValue;
    bool bCheck = SessionSettings->Get(FName("CurrentSessionName"), DebugValue);
    UE_LOG(LogPSGameInstance, Log, TEXT("[Host] SessionSettings 안의 CurrentSessionName = '%s' (bCheck=%d)"), *DebugValue, bCheck);

    // Private 세션 여부에 따라 슬롯 조정. (실제 인원 제한은 NumPublicConnections 사용)
    bIsPrivateSession = bMakePrivate;
    if (bMakePrivate)
    {
        // 비밀번호 필요
        PendingSessionPassword = InPassword;

        // 커스텀 키값으로 비밀번호 정보 저장
        SessionSettings->Set(FName("RequirePassword"), FString("1"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
        SessionSettings->Set(FName("Password"), PendingSessionPassword, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

        // Private 세션이지만 블루프린트상에서 Visibility는 동일하게 광고됨
    }
    else
    {
        PendingSessionPassword.Empty();
        SessionSettings->Set(FName("RequirePassword"), FString("0"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
    }

    // ─────────────── 로그로 세팅값 출력 ───────────────
    UE_LOG(LogPSGameInstance, Log, TEXT("====== CreateSession 세팅값 ======"));
    UE_LOG(LogPSGameInstance, Log, TEXT("  SessionName          = %s"), *CurrentSessionName.ToString());
    UE_LOG(LogPSGameInstance, Log, TEXT("  HostName             = %s"), *HostNick);
    UE_LOG(LogPSGameInstance, Log, TEXT("  bIsLANMatch          = %d"), SessionSettings->bIsLANMatch);
    UE_LOG(LogPSGameInstance, Log, TEXT("  NumPublicConnections = %d"), SessionSettings->NumPublicConnections);
    UE_LOG(LogPSGameInstance, Log, TEXT("  bAllowJoinInProgress = %d"), SessionSettings->bAllowJoinInProgress);
    UE_LOG(LogPSGameInstance, Log, TEXT("  bAllowJoinViaPresence= %d"), SessionSettings->bAllowJoinViaPresence);
    UE_LOG(LogPSGameInstance, Log, TEXT("  bShouldAdvertise     = %d"), SessionSettings->bShouldAdvertise);
    UE_LOG(LogPSGameInstance, Log, TEXT("  bUsesPresence        = %d"), SessionSettings->bUsesPresence);
    UE_LOG(LogPSGameInstance, Log, TEXT("  bUseLobbiesIfAvailable = %d"), SessionSettings->bUseLobbiesIfAvailable);
    UE_LOG(LogPSGameInstance, Log, TEXT("  RequirePassword      = %s"), *FString(bMakePrivate ? TEXT("true") : TEXT("false")));
    if (bMakePrivate)
    {
        UE_LOG(LogPSGameInstance, Log, TEXT("  Password             = %s"), *PendingSessionPassword);
    }
    UE_LOG(LogPSGameInstance, Log, TEXT("===================================="));

    // 실제 세션 생성 요청
    bIsProcessingSession = true;
    OnCreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

    const auto LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), CurrentSessionName, *SessionSettings);

    UE_LOG(LogPSGameInstance, Log, TEXT("SessionName: %s"), *SessionNameString);
    UE_LOG(LogPSGameInstance, Log, TEXT("HostNick: %s"), *HostNick);
    UE_LOG(LogPSGameInstance, Log, TEXT("CreateSession 요청 보냄: Private=%d, Password=%s"), (int)bMakePrivate, *InPassword);
}

void UPS_GameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    // 델리게이트 해제
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
    }
    bIsProcessingSession = false;

    if (!bWasSuccessful)
    {
        UE_LOG(LogPSGameInstance, Error, TEXT("세션 생성 실패"));
        BlueprintJoinSessionsCompleteDelegate.Broadcast(false, TEXT("세션 생성 실패"));
        return;
    }

    UE_LOG(LogPSGameInstance, Log, TEXT("CreateSession 성공: %s"), *SessionName.ToString());

    // 세션 정보(FNamedOnlineSession) 가져오기
    FNamedOnlineSession* NamedSession = SessionInterface->GetNamedSession(SessionName);
    if (!NamedSession)
    {
        UE_LOG(LogPSGameInstance, Warning, TEXT("[OnCreate] NamedSession을 찾을 수 없습니다."));
        return;
    }

    // SessionSettings에 설정된 bUseLobbiesIfAvailable 값 로그
    bool bLobbyFlag = NamedSession->SessionSettings.bUseLobbiesIfAvailable;
    UE_LOG(LogPSGameInstance, Log, TEXT("[OnCreate] SessionSettings.bUseLobbiesIfAvailable = %d"), bLobbyFlag ? 1 : 0);
    UE_LOG(LogPSGameInstance, Log, TEXT("세션 생성 성공: %s"), *SessionName.ToString());

    // 세션 시작(StartSession)은 필요 시 바로 호출 가능
    OnStartSessionCompleteDelegateHandle = SessionInterface->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);
    bool bStarted = SessionInterface->StartSession(SessionName);
    // StartSession() 을 호출하지 않으면, 세션은 계속 “Lobby(대기 상태)”로 남아 있게 되고,
    // 클라이언트가 떠나도 “InProgress”가 아니므로 FindSessions에 계속 노출됨.

    // 생성 후 자동으로 참가된 것이므로, 블루프린트에 “성공” 전달
    BlueprintJoinSessionsCompleteDelegate.Broadcast(true, TEXT("세션 생성 및 참가 완료"));
}

void UPS_GameInstance::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnStartSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

    // Get the Online Subsystem so we can get the Session Interface
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
    if (OnlineSub)
    {
        // Get the Session Interface to clear the Delegate
        IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
        if (Sessions.IsValid())
        {
            // Clear the delegate, since we are done with this call
            Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
        }
    }

    // If the start was successful, we can open a NewMap if we want. Make sure to use "listen" as a parameter!
    if (bWasSuccessful)
    {
        //UGameplayStatics::OpenLevel(GetWorld(), "NewMap", true, "listen");
    }
}

bool UPS_GameInstance::StartGame()
{
    if (!SessionInterface.IsValid())
    {
        UE_LOG(LogPSGameInstance, Error, TEXT("StartGame 실패"));
        return false;
    }

    bool bStarted = SessionInterface->StartSession(CurrentSessionName);
    if (bStarted)
    {
        UE_LOG(LogPSGameInstance, Log, TEXT("게임 시작: 세션이 InProgress 상태로 전환되었습니다."));
        // 이후 게임 씬으로 이동 …
        return true;
    }
    else
    {
        UE_LOG(LogPSGameInstance, Warning, TEXT("StartSession 실패"));
        return false;
    }
}

#pragma endregion

/*
void UPS_GameInstance::DestroySession()
{
    UE_LOG(Project_S, Log, TEXT("Destroy Session"));
    if (SessionInterface.IsValid())
    {
        // Set the Handle
        FDelegateHandle DestroySessionCompleteHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);

        const FName SessionName = NAME_GameSession;

        if (SessionInterface->DestroySession(SessionName))
        {
            UE_LOG(Project_S, Log, TEXT("Requested destroy for session [%s]"), *SessionName.ToString());
            UE_LOG(Project_S, Log, TEXT("Destroy Session Complete"));
        }
        else
        {
            UE_LOG(Project_S, Log, TEXT("Failed to start DestroySession for [%s]"), *SessionName.ToString());
            // 실패 즉시 Delegate 를 해제해줘도 좋습니다:
            //SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteHandle);
        }
    }
}

void UPS_GameInstance::OnDestroySessionComplete(FName SesionName, bool bWasSuccessful)
{
    UE_LOG(Project_S, Log, TEXT("Destroy Session Complete Successfully"));
    if (bWasSuccessful)
    {
        // 세션 생성 성공
        BlueprintDestroySessionsCompleteDelegate.Broadcast();
        UE_LOG(Project_S, Log, TEXT("Destroy Session Complete"));
    }
    else
    {
        // 세션 생성 실패
        UE_LOG(Project_S, Log, TEXT("Start Session Fail"));
    }
}
*/

#pragma region LeaveSession

void UPS_GameInstance::LeaveSession()
{
    /*
    if (!SessionInterface.IsValid())
    {
        UE_LOG(LogPSGameInstance, Warning, TEXT("LeaveSession 불가: SessionInterface invalid"));
        return;
    }

    UE_LOG(LogPSGameInstance, Log, TEXT("LeaveSession CurrentSessionName: %s"), *CurrentSessionName.ToString());

    // 현재 참가한 세션 정보 가져오기
    FNamedOnlineSession* NamedSession = SessionInterface->GetNamedSession(CurrentSessionName);
    if (NamedSession)
    {
        // ─── NamedSession이 있으면 → 호스트 or Dedicated Server 모드 ───
        bool bAmIHost = false;
        IOnlineIdentityPtr Identity = IOnlineSubsystem::Get()->GetIdentityInterface();
        if (Identity.IsValid())
        {
            TSharedPtr<const FUniqueNetId> MyId = Identity->GetUniquePlayerId(0);
            if (MyId.IsValid() && NamedSession->OwningUserId.IsValid())
            {
                bAmIHost = (MyId->ToString() == NamedSession->OwningUserId->ToString());
            }
        }

        EOnlineSessionState::Type SessState = NamedSession->SessionState;
        UE_LOG(LogPSGameInstance, Log, TEXT("Leave Session SessionState: %d"), SessState);

        if (bAmIHost)
        {
            // ① Pending 상태인 로비(Pending)라면 DestroySession 바로 실행
            if (SessState == EOnlineSessionState::Pending)
            {
                UE_LOG(LogPSGameInstance, Log, TEXT("호스트가 Pending 로비를 바로 파괴합니다"));
                OnDestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
                SessionInterface->DestroySession(CurrentSessionName);
                return;
            }

            // ② InProgress 상태라면 EndSession → DestroySession 순서
            if (SessState == EOnlineSessionState::InProgress)
            {
                UE_LOG(LogPSGameInstance, Log, TEXT("호스트가 InProgress 세션을 종료합니다"));
                bIsProcessingSession = true;
                OnEndSessionCompleteDelegateHandle = SessionInterface->AddOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
                SessionInterface->EndSession(CurrentSessionName);
                return;
            }
        }
        else
        {
            // 1) Pending 상태일 경우 → UnregisterPlayer만 시도하고 로컬 정리 (EndSession/DestroySession 호출 X)
            if (NamedSession->SessionState == EOnlineSessionState::Pending)
            {
                UE_LOG(LogPSGameInstance, Log, TEXT("클라이언트(Pending) → UnregisterPlayer() 호출 시도후 로컬 정리"));

                // ① UnregisterPlayer: “나 이 세션 더 이상 참여 안 함” 알림
                //IOnlineIdentityPtr Identity = IOnlineSubsystem::Get()->GetIdentityInterface();
                if (Identity.IsValid() && Identity->GetUniquePlayerId(0).IsValid())
                {
                    TSharedPtr<const FUniqueNetId> MyId = Identity->GetUniquePlayerId(0);

                    // 만약 등록된 플레이어가 맞다면 UnregisterPlayer가 성공합니다.
                    bool bUnregisterSucceeded = SessionInterface->UnregisterPlayer(CurrentSessionName, *MyId);
                    if (!bUnregisterSucceeded)
                    {
                        UE_LOG(LogPSGameInstance, Warning, TEXT("UnregisterPlayer 실패: 플레이어가 세션에 없거나 이미 나간 상태일 수 있습니다."));
                    }
                }

                // ② 로컬 세션 정보만 정리
                SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
                CurrentSessionName = NAME_None;
                BlueprintDestroySessionsCompleteDelegate.Broadcast();
                return;
            }

            // 2) InProgress 상태일 경우 → UnregisterPlayer + EndSession
            if (NamedSession->SessionState == EOnlineSessionState::InProgress)
            {
                UE_LOG(LogPSGameInstance, Log, TEXT("클라이언트(InProgress) → UnregisterPlayer() + EndSession() 호출"));

                // ① UnregisterPlayer
                //IOnlineIdentityPtr Identity = IOnlineSubsystem::Get()->GetIdentityInterface();
                if (Identity.IsValid() && Identity->GetUniquePlayerId(0).IsValid())
                {
                    TSharedPtr<const FUniqueNetId> MyId = Identity->GetUniquePlayerId(0);

                    bool bUnregisterSucceeded = SessionInterface->UnregisterPlayer(CurrentSessionName, *MyId);
                    if (!bUnregisterSucceeded)
                    {
                        UE_LOG(LogPSGameInstance, Warning, TEXT("UnregisterPlayer 실패: 플레이어가 세션에 없거나 이미 나간 상태일 수 있습니다."));
                    }
                }

                // ② EndSession: 로컬 세션 해제
                bIsProcessingSession = true;
                OnEndSessionCompleteDelegateHandle = SessionInterface->AddOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
                SessionInterface->EndSession(CurrentSessionName);
                return;
            }
        }
    }

    // ─── NamedSession == nullptr인 “순수 클라이언트 모드” ───
    // 이 경우 UE API로는 로비 탈퇴를 할 수 없으므로, Steam SDK를 직접 호출하여 LeaveLobby 해야 합니다.
    UE_LOG(LogPSGameInstance, Log, TEXT("클라이언트 모드: Lobby에서 떠납니다 → Steam LeaveLobby 호출"));
    SessionInterface->DestroySession(CurrentSessionName);

    */

    if (!SessionInterface.IsValid())
    {
        UE_LOG(LogPSGameInstance, Warning, TEXT("LeaveSession 불가: SessionInterface invalid"));
        return;
    }

    UE_LOG(LogPSGameInstance, Log, TEXT("LeaveSession 호출됨. CurrentSessionName: %s"), *CurrentSessionName.ToString());

    // 1. 현재 NamedOnlineSession이 있는지 확인
    FNamedOnlineSession* NamedSession = SessionInterface->GetNamedSession(CurrentSessionName);
    if (NamedSession)
    {
        // 1-1. 나(로컬 플레이어)가 호스트인지 확인
        bool bAmIHost = false;
        IOnlineIdentityPtr Identity = IOnlineSubsystem::Get()->GetIdentityInterface();
        TSharedPtr<const FUniqueNetId> MyId = nullptr;
        if (Identity.IsValid())
        {
            MyId = Identity->GetUniquePlayerId(0);
            if (MyId.IsValid() && NamedSession->OwningUserId.IsValid())
            {
                bAmIHost = (MyId->ToString() == NamedSession->OwningUserId->ToString());
            }
        }

        // 1-2. 현재 세션 상태 확인
        EOnlineSessionState::Type SessState = NamedSession->SessionState;
        UE_LOG(LogPSGameInstance, Log, TEXT("LeaveSession - 세션 상태: %d, bAmIHost=%d"), (int32)SessState, bAmIHost ? 1 : 0);

        // 2. 호스트인 경우
        if (bAmIHost)
        {
            // 2-1. Pending 로비 상태라면 바로 DestroySession
            if (SessState == EOnlineSessionState::Pending)
            {
                UE_LOG(LogPSGameInstance, Log, TEXT("호스트(Pending) → 세션을 즉시 파괴합니다."));
                OnDestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
                SessionInterface->DestroySession(CurrentSessionName);
                return;
            }

            // 2-2. InProgress 상태라면 EndSession → DestroySession 순으로 처리
            if (SessState == EOnlineSessionState::InProgress)
            {
                UE_LOG(LogPSGameInstance, Log, TEXT("호스트(InProgress) → 세션 종료(EndSession) 진행"));
                bIsProcessingSession = true;
                OnEndSessionCompleteDelegateHandle = SessionInterface->AddOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
                SessionInterface->EndSession(CurrentSessionName);
                return;
            }

            // (기타 상태: 아마도 Ending, Destroying 등) → 안전하게 DestroySession 호출
            UE_LOG(LogPSGameInstance, Log, TEXT("호스트(기타 상태) → 세션 파괴 요청"));
            OnDestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
            SessionInterface->DestroySession(CurrentSessionName);
            return;
        }
        // 3. 클라이언트인 경우
        else
        {
            // 3-1. Pending 로비 상태라면 UnregisterPlayer만 호출하고 로컬 정리
            if (SessState == EOnlineSessionState::Pending)
            {
                UE_LOG(LogPSGameInstance, Log, TEXT("클라이언트(Pending) → UnregisterPlayer 호출 후 로컬 정리"));

                if (Identity.IsValid() && MyId.IsValid())
                {
                    bool bUnregisterSucceeded = SessionInterface->UnregisterPlayer(CurrentSessionName, *MyId);
                    if (!bUnregisterSucceeded)
                    {
                        UE_LOG(LogPSGameInstance, Warning, TEXT("UnregisterPlayer 실패: 플레이어가 세션에 없거나 이미 나간 상태일 수 있습니다."));
                    }
                    else
                    {
                        UE_LOG(LogPSGameInstance, Log, TEXT("UnregisterPlayer 성공"));
                    }
                }

                // 로컬 변수 정리
                CurrentSessionName = NAME_None;

                //SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
                FOnDestroySessionCompleteDelegate DestroyDel = FOnDestroySessionCompleteDelegate::CreateUObject(
                    this, &UPS_GameInstance::OnDestroySessionComplete
                );
                OnDestroySessionCompleteDelegateHandle =
                    SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroyDel);
                SessionInterface->DestroySession(CurrentSessionName);
                
                BlueprintDestroySessionsCompleteDelegate.Broadcast();
                return;
            }

            // 3-2. InProgress 상태라면 UnregisterPlayer + EndSession
            if (SessState == EOnlineSessionState::InProgress)
            {
                UE_LOG(LogPSGameInstance, Log, TEXT("클라이언트(InProgress) → UnregisterPlayer + EndSession 호출"));

                if (Identity.IsValid() && MyId.IsValid())
                {
                    bool bUnregisterSucceeded = SessionInterface->UnregisterPlayer(CurrentSessionName, *MyId);
                    if (!bUnregisterSucceeded)
                    {
                        UE_LOG(LogPSGameInstance, Warning, TEXT("UnregisterPlayer 실패: 플레이어가 세션에 없거나 이미 나간 상태일 수 있습니다."));
                    }
                }

                bIsProcessingSession = true;
                OnEndSessionCompleteDelegateHandle = SessionInterface->AddOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
                SessionInterface->EndSession(CurrentSessionName);
                return;
            }

            // 3-3. 그 외 상태라면 안전하게 DestroySession 호출
            UE_LOG(LogPSGameInstance, Log, TEXT("클라이언트(기타 상태) → 세션 파괴 요청"));
            OnDestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
            SessionInterface->DestroySession(CurrentSessionName);
            return;
        }
    }

    // 4. NamedSession == nullptr인 “순수 클라이언트 모드”인 경우
    //    (UE 세션 API로는 로비 탈퇴가 불가능하므로 Steam SDK 등을 직접 호출해야 하지만,
    //     간단히 DestroySession 호출하도록 처리)
    UE_LOG(LogPSGameInstance, Log, TEXT("클라이언트 모드(순수) → 세션 파괴 요청"));
    OnDestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
    SessionInterface->DestroySession(CurrentSessionName);
}

void UPS_GameInstance::OnEndSessionComplete(FName SessionName, bool bWasSuccessful)
{
    // 델리게이트 해제
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegateHandle);
    }

    if (!bWasSuccessful)
    {
        UE_LOG(LogPSGameInstance, Warning, TEXT("EndSession 실패: %s"), *SessionName.ToString());
        bIsProcessingSession = false;
        return;
    }

    UE_LOG(LogPSGameInstance, Log, TEXT("EndSession 성공: %s"), *SessionName.ToString());

    // 호스트 여부 재확인
    bool bAmIHost = false;
    IOnlineIdentityPtr Identity = IOnlineSubsystem::Get()->GetIdentityInterface();
    if (Identity.IsValid())
    {
        TSharedPtr<const FUniqueNetId> MyId = Identity->GetUniquePlayerId(0);
        if (MyId.IsValid())
        {
            FString MyNick = Identity->GetPlayerNickname(*MyId);
            FNamedOnlineSession* NamedSession = SessionInterface->GetNamedSession(CurrentSessionName);
            if (NamedSession)
            {
                FString HostNick;
                NamedSession->SessionSettings.Get(FName("HostName"), HostNick);
                bAmIHost = (HostNick == MyNick);
            }
        }
    }

    if (bAmIHost)
    {
        // ─── 호스트라면 실제로 DestroySession 호출 ───
        UE_LOG(LogPSGameInstance, Log, TEXT("호스트가 DestroySession() 호출"));
        OnDestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
            OnDestroySessionCompleteDelegate
        );
        SessionInterface->DestroySession(CurrentSessionName);
    }
    else
    {
        // ─── 클라이언트라면 여기서 세션 끝난 것으로 처리 ───
        UE_LOG(LogPSGameInstance, Log, TEXT("클라이언트 세션 종료 처리 완료"));
        bIsProcessingSession = false;
        CurrentSessionName = NAME_None;
        BlueprintDestroySessionsCompleteDelegate.Broadcast(); // 블루프린트 알림
    }
}

void UPS_GameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    // 델리게이트 해제
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
    }
    bIsProcessingSession = false;
    CurrentSessionName = NAME_None;

    if (!bWasSuccessful)
    {
        UE_LOG(LogPSGameInstance, Warning, TEXT("DestroySession 실패"));
        return;
    }

    UE_LOG(LogPSGameInstance, Log, TEXT("DestroySession 성공: %s"), *SessionName.ToString());    

    // 블루프린트에 “세션 종료” 알림
    BlueprintDestroySessionsCompleteDelegate.Broadcast();

    /*
    if (bWasSuccessful)
    {
        UGameplayStatics::OpenLevel(GetWorld(), "ThirdPersonExampleMap", true);
    }
    */
}

#pragma endregion

#pragma region OnSessionParticipantRemoved

// 호스트 전용: 참가자가 떠났을 때 호출됨
void UPS_GameInstance::OnSessionParticipantRemoved(FName SessionName, const FUniqueNetId& RemovedMemberId)
{
    UE_LOG(LogPSGameInstance, Log, TEXT("OnSessionParticipantRemoved 호출"));
    // 다만, 호스트인지 여부를 간단히 확인하려면
    // 기존 세션 Settings 의 "HostName" 과 내 닉네임 비교 -> 호스트가 맞으면 브로드캐스트

    IOnlineIdentityPtr Identity = IOnlineSubsystem::Get()->GetIdentityInterface();
    FString MyNick;
    if (Identity.IsValid() && Identity->GetUniquePlayerId(0).IsValid())
    {
        MyNick = Identity->GetPlayerNickname(0);
    }

    // 호스트만 이 콜백에 유의하며, 자신이 호스트일 때만 블루프린트 알림
    FNamedOnlineSession* NamedSession = SessionInterface->GetNamedSession(CurrentSessionName);
    if (NamedSession)
    {
        FString HostNick;
        NamedSession->SessionSettings.Get(FName("HostName"), HostNick);
        bool bAmIHost = (HostNick == MyNick);
        if (bAmIHost)
        {
            // 떠난 사용자 닉네임 얻기
            FString LeftNick = TEXT("Unknown");
            // RemovedMemberId.ToString() 대신, Identity->GetPlayerNicknameForUniqueNetId 로 실제 닉네임 얻기 시도
            TSharedPtr<const FUniqueNetId> RemovedPtr = RemovedMemberId.AsShared();
            if (RemovedPtr.IsValid() && Identity.IsValid())
            {
                LeftNick = Identity->GetPlayerNickname(*RemovedPtr);
            }

            UE_LOG(LogPSGameInstance, Log, TEXT("호스트에게 알림: 플레이어 '%s' 가 세션을 떠났습니다."), *LeftNick);
            OnPlayerLeftSession.Broadcast(LeftNick);

            // ─────── 여기서 “joinable” 상태를 다시 켜 주기 ───────
            // 세션 설정을 복사해서 수정한 뒤 UpdateSession 호출
            FOnlineSessionSettings NewSettings = NamedSession->SessionSettings;

            // joinable 상태로 컬백 다시 세팅
            NewSettings.bShouldAdvertise = true;
            NewSettings.bAllowJoinInProgress = true;

            // (선택) 만약 비밀번호를 유지해야 한다면, 커스텀 필드는 그대로 가져갑니다.
            // 예: "RequirePassword", "Password" 등은 이미 NewSettings에 복사되어 있습니다.

            // UpdateSession 호출
            SessionInterface->UpdateSession(SessionName, NewSettings);
            UE_LOG(LogPSGameInstance, Log, TEXT("호스트 로비 설정을 다시 joinable 상태로 갱신했습니다: %s"), SessionName);
        }
    }
}

#pragma endregion

/*
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

void UPS_GameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(Project_S, Log, TEXT("GameInstance : %s, Find Sessions Complete Successfully"), *this->GetName());
        UE_LOG(Project_S, Log, TEXT("OnFindSessionsComplete called. SessionSearch pointer: %p"), SessionSearch.Get());
        // 성공
        if (SessionSearch.IsValid())
        {
            int cnt = 1;
            TArray<FBlueprintSessionResult> BlueprintSessionResults;
            for (const auto& Result : SessionSearch->SearchResults)
            {
                if (Result.IsValid())
                {
                    UE_LOG(Project_S, Warning, TEXT("cnt = %d"), cnt);
                    cnt++;
                    FBlueprintSessionResult BlueprintSessionResult;
                    BlueprintSessionResult.OnlineResult = Result;
                    BlueprintSessionResults.Add(BlueprintSessionResult);
                }
                else
                {
                    UE_LOG(Project_S, Warning, TEXT("SearchResult not Valid"));
                }
            }

            UE_LOG(Project_S, Warning, TEXT("BlueprintSessionResults.size() = %d"), BlueprintSessionResults.Num());
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
*/

#pragma region FindSessions

void UPS_GameInstance::FindSessions()
{
    if (!SessionInterface.IsValid())
    {
        UE_LOG(LogPSGameInstance, Error, TEXT("Cannot FindSessions: SessionInterface invalid"));
        BlueprintFindSessionsCompleteDelegate.Broadcast(TArray<FBlueprintSessionResult>(), false);
        return;
    }

    if (bIsProcessingSession)
    {
        UE_LOG(LogPSGameInstance, Warning, TEXT("이미 세션 작업 중이므로 검색 불가"));
        return;
    }

    // 세션 검색 객체 생성
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    SessionSearch->MaxSearchResults = MAX_SESSION_RESULTS;
    SessionSearch->bIsLanQuery = false;
    SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

    bIsProcessingSession = true;
    OnFindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

    const auto LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());

    UE_LOG(LogPSGameInstance, Log, TEXT("FindSessions 요청 보냄"));
}

void UPS_GameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
    // 델리게이트 해제
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
    }
    bIsProcessingSession = false;

    TArray<FBlueprintSessionResult> ValidResults;

    if (!SessionSearch.IsValid())
    {
        UE_LOG(LogPSGameInstance, Warning, TEXT("FindSessions 실패 또는 유효하지 않은 검색 결과"));
        BlueprintFindSessionsCompleteDelegate.Broadcast(ValidResults, false);
        return;
    }// ‘검색 결과 배열의 개수’를 먼저 확인
    const int32 FoundCount = SessionSearch->SearchResults.Num();
    UE_LOG(LogPSGameInstance, Log, TEXT("OnFindSessionsComplete: bWasSuccessful=%d, SearchResults.Num()=%d"), bWasSuccessful ? 1 : 0, FoundCount);

    // 검색 결과가 하나도 없으면 실패 처리
    if (FoundCount <= 0)
    {
        UE_LOG(LogPSGameInstance, Warning, TEXT("검색 결과가 없습니다."));
        BlueprintFindSessionsCompleteDelegate.Broadcast(ValidResults, false);
        return;
    }

    // (Optional) bWasSuccessful이 false이지만, FoundCount > 0이면 “부분 성공”으로 간주
    if (!bWasSuccessful)
    {
        UE_LOG(LogPSGameInstance, Warning,
            TEXT("Steam FindSessions 내부적으로 bWasSuccessful=false이지만, %d개 로비가 검색되었습니다. 결과를 계속 처리합니다."),
            FoundCount);
    }

    // 3) 실제로 남은 슬롯이 있는 세션만 골라서 ValidResults에 담기
    for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
    {
        if (SearchResult.Session.NumOpenPublicConnections <= 0)
        {
            // 빈 슬롯이 없는 세션이면 건너뛴다.
            continue;
        }
        FBlueprintSessionResult BlueprintResult;
        BlueprintResult.OnlineResult = SearchResult;
        ValidResults.Add(BlueprintResult);
    }

    UE_LOG(LogPSGameInstance, Log, TEXT("검색 완료: 유효한 세션 %d개"), ValidResults.Num());
    BlueprintFindSessionsCompleteDelegate.Broadcast(ValidResults, true);
}

#pragma endregion

/*
void UPS_GameInstance::JoinSession(int Index)
{
    UE_LOG(Project_S, Log, TEXT("Join Session Start"));
    UE_LOG(Project_S, Log, TEXT("Index: %d"), Index);

    if (SessionInterface.IsValid())
    {
        // Set the Handle
        SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

        // idx: 원하는 Session의 index
        //int idx = 0;
        const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
        auto SessionName = FName(SessionSearch->SearchResults[Index].Session.OwningUserName);
        auto Result = SessionSearch->SearchResults[Index];
        SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), SessionName, Result);
        UE_LOG(Project_S, Log, TEXT("Join Session Complete"));
    }
    else
    {
        UE_LOG(Project_S, Log, TEXT("Join Session Error"));
    }
}

void UPS_GameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    UE_LOG(Project_S, Log, TEXT("GameInstance : %s, Find Sessions Complete Successfully"), *this->GetName());
    UE_LOG(Project_S, Log, TEXT("OnFindSessionsComplete called. SessionSearch pointer: %p"), SessionSearch.Get());

    switch (Result)
    {
    /** The join worked as expected
    case EOnJoinSessionCompleteResult::Success:
        if (SessionInterface.IsValid())
        {
            APlayerController* const PlayerController = GetFirstLocalPlayerController();
            FString TravelURL;

            if (PlayerController && SessionInterface->GetResolvedConnectString(SessionName, TravelURL))
            {
                PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
            }
            else
            {
                UE_LOG(Project_S, Log, TEXT("Failed to resolve connect string for session %s"), *SessionName.ToString());
                BlueprintJoinSessionsCompleteDelegate.Broadcast();
            }
        }

        break;
    /** There are no open slots to join 
    case EOnJoinSessionCompleteResult::SessionIsFull:
        UE_LOG(Project_S, Log, TEXT("Cannot join session %s: Session is full"), *SessionName.ToString());
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("방이 가득 찼습니다."));
        }
        BlueprintJoinSessionsCompleteDelegate.Broadcast();
        break;
    /** The session couldn't be found on the service 
    case EOnJoinSessionCompleteResult::SessionDoesNotExist:
        UE_LOG(Project_S, Log, TEXT("Cannot join session %s: Session does not exist"), *SessionName.ToString());
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("세션을 찾을 수 없습니다."));
        }
        BlueprintJoinSessionsCompleteDelegate.Broadcast();
        break;
    /** There was an error getting the session server's address 
    case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
        UE_LOG(Project_S, Log, TEXT("Cannot join session %s: Could not retrieve server address"), *SessionName.ToString());
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("서버 주소를 가져올 수 없습니다."));
        }
        BlueprintJoinSessionsCompleteDelegate.Broadcast();
        break;
    /** The user attempting to join is already a member of the session 
    case EOnJoinSessionCompleteResult::AlreadyInSession:
        UE_LOG(Project_S, Log, TEXT("Player is already in session %s"), *SessionName.ToString());
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("이미 해당 세션에 접속되어 있습니다."));
        }
        BlueprintJoinSessionsCompleteDelegate.Broadcast();
        break;
    /** An error not covered above occurred 
    case EOnJoinSessionCompleteResult::UnknownError:
        UE_LOG(Project_S, Log, TEXT("Unknown error joining session %s, Result=%d"), *SessionName.ToString(), static_cast<int32>(Result));
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("알 수 없는 오류로 접속에 실패했습니다."));
        }
        BlueprintJoinSessionsCompleteDelegate.Broadcast();
        break;
    }

    bIsJoining = false;
}
*/

#pragma region JoinSession

void UPS_GameInstance::JoinSession(int32 SessionIndex, const FString& InPassword)
{
    if (!SessionInterface.IsValid() || !SessionSearch.IsValid())
    {
        UE_LOG(LogPSGameInstance, Error, TEXT("Cannot JoinSession: Invalid Interface/Search"));
        BlueprintJoinSessionsCompleteDelegate.Broadcast(false, TEXT("세션 시스템 오류"));
        return;
    }

    if (bIsProcessingSession)
    {
        UE_LOG(LogPSGameInstance, Warning, TEXT("이미 세션 작업 중이므로 참가 불가"));
        return;
    }

    if (SessionIndex < 0 || SessionIndex >= SessionSearch->SearchResults.Num())
    {
        UE_LOG(LogPSGameInstance, Warning, TEXT("잘못된 세션 인덱스: %d"), SessionIndex);
        BlueprintJoinSessionsCompleteDelegate.Broadcast(false, TEXT("잘못된 세션 인덱스"));
        return;
    }

    const FOnlineSessionSearchResult& ChosenResult = SessionSearch->SearchResults[SessionIndex];
    UE_LOG(LogPSGameInstance, Log, TEXT("Join 세션 인덱스: %d"), SessionIndex);

    // 검색된 결과에서 SessionIdStr (세션 이름)을 불러와 CurrentSessionName에 저장
    bool bHasCustomName = false;
    FString CustomName;
    bHasCustomName = ChosenResult.Session.SessionSettings.Get(FName("CurrentSessionName"), CustomName);
    UE_LOG(LogPSGameInstance, Log, TEXT("Join CustomName: %s"), *CustomName);
    if (bHasCustomName && !CustomName.IsEmpty())
    {
        CurrentSessionName = FName(*CustomName);
    }
    else
    {
        // 기존처럼 fallback: GetSessionIdStr() 를 넣어두거나, 오류 처리
        CurrentSessionName = FName(*ChosenResult.Session.GetSessionIdStr());
    }
    UE_LOG(LogPSGameInstance, Log, TEXT("Join CurrentSessionName: %s"), *CurrentSessionName.ToString());

    // 선택된 세션이 Private인지(커스텀 항목 “RequirePassword” 확인)
    bool bRequirePwd = false;
    FString StoredPwd;
    ChosenResult.Session.SessionSettings.Get(FName("RequirePassword"), bRequirePwd);
    if (bRequirePwd)
    {
        // 실제로 저장된 비밀번호를 읽어와서 비교
        ChosenResult.Session.SessionSettings.Get(FName("Password"), StoredPwd);
        if (StoredPwd != InPassword)
        {
            UE_LOG(LogPSGameInstance, Warning, TEXT("비밀번호 불일치: 입력='%s', 실제='%s'"), *InPassword, *StoredPwd);
            BlueprintJoinSessionsCompleteDelegate.Broadcast(false, TEXT("비밀번호가 틀렸습니다"));
            return;
        }
    }

    // Join 요청
    bIsProcessingSession = true;
    JoinAttemptPassword = InPassword; // 필요 시 보관
    OnJoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

    const auto LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), CurrentSessionName, ChosenResult);

    UE_LOG(LogPSGameInstance, Log, TEXT("CurrentSessionName: %s"), *CurrentSessionName.ToString());
    UE_LOG(LogPSGameInstance, Log, TEXT("JoinSession 요청 보냄"));
}

void UPS_GameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    // 델리게이트 해제
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
    }
    bIsProcessingSession = false;

    if (Result != EOnJoinSessionCompleteResult::Success)
    {
        FString Reason = TEXT("세션 참가 실패");
        switch (Result)
        {
        case EOnJoinSessionCompleteResult::SessionIsFull:
            Reason = TEXT("세션이 가득 찼습니다");
            break;
        case EOnJoinSessionCompleteResult::SessionDoesNotExist:
            Reason = TEXT("세션을 찾을 수 없습니다");
            break;
        case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
            Reason = TEXT("서버 주소를 가져올 수 없습니다");
            break;
        case EOnJoinSessionCompleteResult::AlreadyInSession:
            Reason = TEXT("이미 세션에 참여 중입니다");
            break;
        default:
            Reason = TEXT("알 수 없는 오류로 참가 실패");
            break;
        }
        UE_LOG(LogPSGameInstance, Warning, TEXT("JoinSession 실패: %s"), *Reason);
        BlueprintJoinSessionsCompleteDelegate.Broadcast(false, Reason);
        return;
    }

    // 클라이언트 입장에서 현재 세션 정보를 가져와서 디버깅용으로 출력
    FNamedOnlineSession* NamedSession = SessionInterface->GetNamedSession(SessionName);
    if (NamedSession && NamedSession->SessionInfo.IsValid())
    {
        FString SessionIdStr = NamedSession->SessionInfo->ToString();
        UE_LOG(LogPSGameInstance, Log, TEXT("  [OnJoin] 세션 ID (ToString) = %s"), *SessionIdStr);
    }

    // 세션에 등록(RegisterPlayer)” 시도
    {
        IOnlineIdentityPtr Identity = IOnlineSubsystem::Get()->GetIdentityInterface();
        if (Identity.IsValid() && Identity->GetUniquePlayerId(0).IsValid())
        {
            TSharedPtr<const FUniqueNetId> MyId = Identity->GetUniquePlayerId(0);

            // RegisterPlayer 호출: 세션 내부에 “현재 클라이언트”를 등록해 달라고 요청
            bool bRegistered = SessionInterface->RegisterPlayer(SessionName, *MyId, true);
            if (bRegistered)
            {
                UE_LOG(LogPSGameInstance, Log, TEXT("클라이언트 RegisterPlayer 성공: %s"), *MyId->ToString());
            }
            else
            {
                UE_LOG(LogPSGameInstance, Warning, TEXT("클라이언트 RegisterPlayer 실패(Local 이미 등록되었거나, 서버가 닫힘)"));
            }
        }
        else
        {
            UE_LOG(LogPSGameInstance, Warning, TEXT("RegisterPlayer 불가: Identity 또는 UniquePlayerId invalid"));
        }
    }

    // 참가 성공: Travel URL 얻어서 이동
    FString TravelURL;
    auto LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (SessionInterface->GetResolvedConnectString(SessionName, TravelURL))
    {
        UE_LOG(LogPSGameInstance, Log, TEXT("세션 참가 성공, TravelURL=%s"), *TravelURL);
        APlayerController* const PC = GetFirstLocalPlayerController();
        //APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
        if (PC)
        {
            PC->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
        }
        BlueprintJoinSessionsCompleteDelegate.Broadcast(true, TEXT("세션 참가 완료"));
    }
    else
    {
        UE_LOG(LogPSGameInstance, Error, TEXT("GetResolvedConnectString 실패"));
        BlueprintJoinSessionsCompleteDelegate.Broadcast(false, TEXT("서버 연결 실패"));
    }
}

#pragma endregion

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