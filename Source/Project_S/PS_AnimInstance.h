// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Project_S.h"
#include "Animation/AnimInstance.h"
#include "PS_AnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);

/**
 * 
 */
UCLASS()
class PROJECT_S_API UPS_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPS_AnimInstance();

	// Override functions
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// User functions
	void JumpToMontageSection(UAnimMontage* Montage, int NewSection);

	// Delegates
	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnAttackHitCheckDelegate OnAttackHitCheck;

	// Montage reference
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DodgeMontage;
	
private:
	// Anim notify functions
	UFUNCTION()
	void AnimNotify_AttackHitCheck();

	UFUNCTION()
	void AnimNotify_NextAttackCheck();

	// User functions
	void LoadAnimMontage(UAnimMontage*& Montage, const TCHAR* Path);
	FName GetMontageSectionName(UAnimMontage* Montage, int Section);

	// Animation variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", meta = (AllowPrivateAccess = "true"))
	float MaxWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", meta = (AllowPrivateAccess = "true"))
	FVector2D CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", meta = (AllowPrivateAccess = "true"))
	bool IsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadONly, Category = "Pawn", meta = (AllowPrivateAccess = "true"))
	FRotator ControlRotation;

	const float MAX_ROTATION_ROLL = 60.0f;
	const float MIN_ROTATION_ROLL = -45.0f;
	const float MAX_ROTATION_YAW = 75.0f;
	const float MIN_ROTATION_YAW = -75.0f;

	/*
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, meta = (AllowPrivateAccess = "true"))
	bool IsSprinting;
	*/
};
