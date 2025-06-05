// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Project_S.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h" 
#include "OnlineSessionSettings.h"
#include "FindSessionsCallbackProxy.h"
#include "PS_GameInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPSGameInstance, Log, All);

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBlueprintFindSessionsCompleteDelegate, const TArray<FBlueprintSessionResult>&, Results);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBlueprintJoinSessionsCompleteDelegate);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBlueprintDestroySessionsCompleteDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBlueprintFindSessionsCompleteDelegate, const TArray<FBlueprintSessionResult>&, Results, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBlueprintJoinSessionsCompleteDelegate, bool, bSuccess, const FString&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBlueprintDestroySessionsCompleteDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBlueprintPlayerLeftSessionDelegate, const FString&, PlayerName);

/**
 * 
 */
UCLASS()
class PROJECT_S_API UPS_GameInstance : public UGameInstance
{
	GENERATED_BODY()

	UPS_GameInstance();
	
public:
	virtual void Init() override;
	void SetMap(uint8 InMap);
	void SetStage(uint8 InStage);
	void SetLife(uint8 InLife);
	void DeductLife();
	void SetIsGameStart(bool NewBool);

	uint8 GetMap() const;
	uint8 GetStage() const;
	uint8 GetLife() const;
	bool IsGameStart() const;

	bool StartGame();

	/*
	// Delegates
	FOnCreateSessionCompleteDelegate OnCreateSesionCompleteDelegate;
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	*/

	/*
	UPROPERTY(BlueprintAssignable, Category = "Sessions")
	FBlueprintFindSessionsCompleteDelegate BlueprintFindSessionsCompleteDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Sessions")
	FBlueprintJoinSessionsCompleteDelegate BlueprintJoinSessionsCompleteDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Sessions")
	FBlueprintDestroySessionsCompleteDelegate BlueprintDestroySessionsCompleteDelegate;
	*/

protected:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	/*
	UFUNCTION(BlueprintCallable)
	void CreateSession();

	UFUNCTION(BlueprintCallable)
	void DestroySession();

	UFUNCTION(BlueprintCallable)
	void FindSession();

	UFUNCTION(BlueprintCallable)
	void JoinSession(int Index);
	*/

	// Sessions
	/** 세션 생성
	 *  @param bMakePrivate   : true 이면 비밀번호 필요(Private), false 이면 공개 세션(Public)
	 *  @param InPassword     : 비밀번호 (Public 세션이면 빈 문자열)
	 */
	UFUNCTION(BlueprintCallable, Category = "Sessions")
	void CreateSession(bool bMakePrivate, const FString& InPassword);

	/** 세션 검색 */
	UFUNCTION(BlueprintCallable, Category = "Sessions")
	void FindSessions();

	/** 세션 참가
	 *  @param SessionIndex  : FindSessions() 결과 배열에서의 인덱스
	 *  @param InPassword    : Private 세션일 때 사용자가 입력한 비밀번호
	 */
	UFUNCTION(BlueprintCallable, Category = "Sessions")
	void JoinSession(int32 SessionIndex, const FString& InPassword);

	/** 현재 속해 있는 세션에서 퇴장 */
	UFUNCTION(BlueprintCallable, Category = "Sessions")
	void LeaveSession();

	void OnCreateSessionComplete(FName SesionName, bool bWasSuccessful);
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);
	void OnEndSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SesionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnSessionParticipantRemoved(FName SessionName, const FUniqueNetId& RemovedMemberId);

	// Session Event Dispatcher
	/** FindSessions() 후 결과가 준비되면 실행 */
	UPROPERTY(BlueprintAssignable, Category = "Sessions")
	FBlueprintFindSessionsCompleteDelegate BlueprintFindSessionsCompleteDelegate;

	/** JoinSession() 시도 후 결과를 알림 */
	UPROPERTY(BlueprintAssignable, Category = "Sessions")
	FBlueprintJoinSessionsCompleteDelegate BlueprintJoinSessionsCompleteDelegate;

	/** 세션이 완전히 파괴(호스트 퇴장)되거나, 자신이 파괴한 경우 알림 */
	UPROPERTY(BlueprintAssignable, Category = "Sessions")
	FBlueprintDestroySessionsCompleteDelegate BlueprintDestroySessionsCompleteDelegate;

	/** 일반 플레이어가 세션을 떠났을 때 호스트에게 알리는 이벤트 */
	UPROPERTY(BlueprintAssignable, Category = "Sessions")
	FBlueprintPlayerLeftSessionDelegate OnPlayerLeftSession;

	// Delegates
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;

	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;

	FOnEndSessionCompleteDelegate OnEndSessionCompleteDelegate;
	FDelegateHandle OnEndSessionCompleteDelegateHandle;

	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	FOnSessionParticipantRemovedDelegate OnSessionParticipantRemovedDelegate;
	FDelegateHandle OnSessionParticipantRemovedDelegateHandle;

	

private:
	uint8 Map;
	uint8 Stage;
	uint8 Life;
	bool bIsGameStart;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn", meta = (AllowPrivateAccess = "true"))
	//bool bIsJoining;

	/** 세션 생성 시 커스텀으로 지정한 비밀번호 (Private 세션이면 유효) */
	FString PendingSessionPassword;

	/** JoinSession 호출 시 입력받은 비밀번호 (Private 세션 검증용) */
	FString JoinAttemptPassword;

	/** CreateSession 호출 시, 세션을 Private으로 생성할지 여부 */
	bool bIsPrivateSession;

	/** 현재 자신이 참가한 세션 이름 (FName) */
	FName CurrentSessionName;

	/** 세션 생성/참가/검색 중 상태를 막기 위한 플래그 */
	bool bIsProcessingSession;

	/** 최대 인원 수 (2명) */
	static constexpr int32 MAX_PUBLIC_CONNECTIONS = 2;
	static constexpr int32 NUM_PRIVATE_CONNECTIONS = 0; // Private 세션일 때 대기 슬롯 수

	/** 최대 세션 검색 수 */
	static constexpr int32 MAX_SESSION_RESULTS = 10;
};
