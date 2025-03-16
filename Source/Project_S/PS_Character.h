// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Project_S.h"
#include "PS_BasePickup.h"
#include "GameFramework/Character.h"
#include "PS_Character.generated.h"

UENUM(BlueprintType)
enum class EDirection : uint8
{
	N UMETA(DisplayName = "N"),
	W UMETA(DisplayName = "W"),
	WA UMETA(DisplayName = "WA"),
	A UMETA(DisplayName = "A"),
	SA UMETA(DisplayName = "SA"),
	S UMETA(DisplayName = "S"),
	SD UMETA(DisplayName = "SD"),
	D UMETA(DisplayName = "D"),
	WD UMETA(DisplayName = "WD")
};

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
	void SetEmotionID(int NewEmotionID);

	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	// Input action functions
	void Move(const struct FInputActionValue& Value);
	void MoveEnd(const struct FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void SprintStart(const FInputActionValue& Value);
	void SprintEnd(const FInputActionValue& Value);
	void Interact(const FInputActionValue& Value);
	void JumpStart(const FInputActionValue& Value);
	void JumpEnd(const FInputActionValue& Value);
	void SetDitherAlpha(const float Value);

	// Turn RPC functions
	void RotateActor(FRotator NewRotator);

	UFUNCTION(Server, Reliable)
	void RotateActor_Server(FRotator NewRotator);

	UFUNCTION(NetMulticast, Reliable)
	void RotateActor_Client(FRotator NewRotator);

	void SetHeadRotator(FRotator NewRotator);

	UFUNCTION(Server, Reliable)
	void SetHeadRotator_Server(FRotator NewRotator);

	UFUNCTION(NetMulticast, Reliable)
	void SetHeadRotator_Client(FRotator NewRotator);

	// Sprint RPC functions
	UFUNCTION(Server, Reliable)
	void SprintStart_Server();

	UFUNCTION(NetMulticast, Reliable)
	void SprintStart_Client();

	UFUNCTION(Server, Reliable)
	void SprintEnd_Server();

	UFUNCTION(NetMulticast, Reliable)
	void SprintEnd_Client();

	// Interact RPC functions
	UFUNCTION(Server, Reliable)
	void Interact_Server();

	// Attack RPC functions
	void Attack(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Attack_Server();

	void HandleAttack();

	// Dodge RPC funtions
	void DodgeDirectionStart(const FInputActionValue& Value);

	void DodgeDirectionEnd(const FInputActionValue& Value);

	void Dodge(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void Dodge_Server(const FVector Value);

	UFUNCTION(NetMulticast, Reliable)
	void Dodge_Client(const FVector Value);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dodge")
	void OnDodge(const FVector& LaunchVector);

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

	// Dodge variables
	EDirection Dodge_Direction;

	void Emotion1(const FInputActionValue& Value);
	void Emotion2(const FInputActionValue& Value);
	void Emotion3(const FInputActionValue& Value);
	void Emotion4(const FInputActionValue& Value);
	void Emotion5(const FInputActionValue& Value);
	void Emotion6(const FInputActionValue& Value);

	// EmotionID 변수 (Replicated)
	UPROPERTY(Replicated)
	int EmotionID = 0;

	// 오브젝트 클래스 배열
	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	TArray<TSubclassOf<AActor>> SpawnableObjectClasses;

	// 서버에서 EmotionID를 설정하는 함수
	UFUNCTION(Server, Reliable, WithValidation)
	void SetEmotionID_Server(int NewEmotionID);

	// 감정(Emotion)과 관련된 오브젝트를 스폰하는 함수
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnEmotion();

	// 서버에서 Emotion을 기반으로 스폰하는 함수
	UFUNCTION(NetMulticast, Reliable)
	void SpawnObject(const FVector& SpawnLocation, const FRotator& SpawnRotation, TSubclassOf<AActor> ObjectClass);

	// 서버에서 EmotionID를 기반으로 오브젝트를 스폰하는 함수
	UFUNCTION(Server, Reliable, WithValidation)
	void SpawnObject_Server();


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
	TObjectPtr<class UInputAction> MoveEndAction;

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
	TObjectPtr<class UInputAction> DodgeDirectionStartAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> DodgeDirectionEndAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> DodgeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> Emotion1Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> Emotion2Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> Emotion3Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> Emotion4Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> Emotion5Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> Emotion6Action;



	// Data table reference variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	class UDataTable* CharacterDataTable;

	// PS_CharacterStats pointer variables
	struct FPS_CharacterStats* CharacterStats;

	// Interactable Actor pointer variables
	UPROPERTY()
	AActor* InteractableActor;

	// Grabable Actor pointer variables
	UPROPERTY()
	AActor* GrabableActor;

	UPROPERTY()
	AActor* GrabbedActor;

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

	// Dodge variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge", meta = (AllowPrivateAccess = "true"))
	bool bIsDodging;

	// Actor Turn variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn", meta = (AllowPrivateAccess = "true"))
	bool bIsMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn", meta = (AllowPrivateAccess = "true"))
	bool bIsTurning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn", meta = (AllowPrivateAccess = "true"))
	FRotator HeadRotator;

	const float MAX_ROTATION_ROLL = 60.0f;
	const float MIN_ROTATION_ROLL = -45.0f;
	const float MAX_ROTATION_YAW = 75.0f;
	const float MIN_ROTATION_YAW = -75.0f;

	// Material variables
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Opacity", meta = (AllowPrivateAccess = "true"))
	float DitherAlpha;

	/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn", meta = (AllowPrivateAccess = "true"))
	bool bIsTurning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn", meta = (AllowPrivateAccess = "true"))
	bool bIsTurning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn", meta = (AllowPrivateAccess = "true"))
	bool bIsTurning;
	*/
public:
	// Getter functions
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE FPS_CharacterStats* GetCharacterStats() const { return CharacterStats; }
	FORCEINLINE FRotator GetHeadRotator() const { return HeadRotator; }
};
