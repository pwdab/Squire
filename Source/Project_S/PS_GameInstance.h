// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Project_S.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "FindSessionsCallbackProxy.h"
#include "PS_GameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBlueprintFindSessionsCompleteDelegate, const TArray<FBlueprintSessionResult>&, Results);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBlueprintJoinSessionsCompleteDelegate);

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

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	// Delegates
	FOnCreateSessionCompleteDelegate OnCreateSesionCompleteDelegate;
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Sessions")
	FBlueprintFindSessionsCompleteDelegate BlueprintFindSessionsCompleteDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Sessions")
	FBlueprintJoinSessionsCompleteDelegate BlueprintJoinSessionsCompleteDelegate;

protected:
	UFUNCTION(BlueprintCallable)
	void CreateSession();

	UFUNCTION(BlueprintCallable)
	void DestroySession();

	UFUNCTION(BlueprintCallable)
	void FindSession();

	UFUNCTION(BlueprintCallable)
	void JoinSession(int Index);

	void OnCreateSessionComplete(FName SesionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SesionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

private:
	uint8 Map;
	uint8 Stage;
	uint8 Life;
	bool bIsGameStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn", meta = (AllowPrivateAccess = "true"))
	bool bIsJoining;
};
