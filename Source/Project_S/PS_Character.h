// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Project_S.h"
#include "PS_BasePickup.h"
#include "GameFramework/Character.h"
#include "PS_Character.generated.h"

//DECLARE_DELEGATE_OneParam(FOnSprint, bool);
UCLASS()
class PROJECT_S_API APS_Character : public ACharacter
{
	GENERATED_BODY()

public:
	APS_Character();

	// Override functions
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// User functions
	void UpdateCharacterStats();
	bool CanSetWeapon(EHand Hand);
	void SetWeapon(class APS_Weapon* NewWeapon, EHand NewHand);

protected:
	virtual void BeginPlay() override;

	// Input action functions
	void Move(const struct FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void SprintStart(const FInputActionValue& Value);
	void SprintEnd(const FInputActionValue& Value);
	void Interact(const FInputActionValue& Value);
	void JumpStart(const FInputActionValue& Value);
	void JumpEnd(const FInputActionValue& Value);

	// Sprint RPC functions
	UFUNCTION(Server, Reliable)
	void SprintStart_Server();

	UFUNCTION(NetMulticast, Reliable)
	void SprintStart_Client();

	UFUNCTION(Server, Reliable)
	void SprintEnd_Server();

	UFUNCTION(NetMulticast, Reliable)
	void SprintEnd_Client();

	// Attack RPC functions
	void Attack(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Attack_Server();

	void HandleAttack();

	void Dodge(const FInputActionValue& Value);

	// Montage RPC functions
	UFUNCTION()
	void PlayMontage(UAnimMontage* Montage);

	UFUNCTION(NetMulticast, Reliable)
	void PlayMontage_Client(UAnimMontage* Montage);

	UFUNCTION()
	void JumpToMontageSection(UAnimMontage* Montage, int NewSection);

	UFUNCTION(NetMulticast, Reliable)
	void JumpToMontageSection_Client(UAnimMontage* Montage, int NewSection);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(NetMulticast, Reliable)
	void OnMontageEnded_Client(UAnimMontage* Montage, bool bInterrupted);

	// User functions
	void AttackStartComboState();
	void AttackEndComboState();

	// Attack variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackRange = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackDuration = 1.0f;

private:
	// Component variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

	// Input components
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> DodgeAction;

	// Data table reference variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	class UDataTable* CharacterDataTable;

	// PS_CharacterStats pointer variables
	struct FPS_CharacterStats* CharacterStats;

	// Weapon variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class APS_Weapon* CurrentLeftWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class APS_Weapon* CurrentRightWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	EHand CurrentHand;

	// AnimInstance variables
	UPROPERTY()
	class UPS_AnimInstance* PS_AnimInstance;

	// Attack variables
	//UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
	bool bIsAttacking;

	//UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
	bool bCanNextCombo;

	//UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
	bool bIsComboInputOn;

	//UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
	int CurrentCombo;

	//UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
	int MaxCombo;

public:
	// Getter functions
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE FPS_CharacterStats* GetCharacterStats() const { return CharacterStats; }
};
