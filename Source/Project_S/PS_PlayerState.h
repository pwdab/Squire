// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Project_S.h"
#include "GameFramework/PlayerState.h"
#include "Delegates/DelegateCombinations.h"
#include "PS_PlayerState.generated.h"


// Declare Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWordChanged, FString, NewWord);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWordSelected, bool, NewSelection);
/**
 * 
 */
UCLASS()
class PROJECT_S_API APS_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWordChanged OnWordChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWordSelected OnWordSelected;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = "OnRep_Word", Category = "HUD")
	FString SelectedWord;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = "OnRep_WordSelected", Category = "HUD")
	bool bHasSelectedWord;

	UFUNCTION()
	void OnRep_Word(FString OldValue) const;

	UFUNCTION()
	void OnRep_WordSelected(bool OldValue) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	void InitSelectedWord();

	UFUNCTION(Server, Reliable, Category = "HUD")
	void InitSelectedWord_Server();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	void UpdateSelectedWord(const FString& Word);

	UFUNCTION(Server, Reliable, Category = "HUD")
	void UpdateSelectedWord_Server(const FString& Word);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	FString GetSelectedWord();

	bool HasSelectedWord() const;

protected:
	virtual void BeginPlay() override;

public:
	// Getter functions
	FORCEINLINE bool GetHasSelectedWord() const { return bHasSelectedWord; }
};
