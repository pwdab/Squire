// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PS_PlayerState.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVelocityChanged, float, NewVelocity);

UCLASS()
class PROJECT_S_API APS_PlayerState : public APlayerState
{
	GENERATED_BODY()
	/*
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = "OnRep_Velocity", Category = "Velocity")
	float Velocity = 0.0f;

	UPROPERTY(BlueprintAssignable, Category = "Velocity")
	FOnVelocityChanged OnVelocityChanged;

	UFUNCTION()
	void OnRep_Velocity(float OldValue) const;

public:
	UFUNCTION(BlueprintCallable, Category = "Velocity")
	void UpdateVelocity(float Value);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	*/
};
