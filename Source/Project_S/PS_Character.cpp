// Fill out your copyright notice in the Description page of Project Settings.

#include "PS_Character.h"

// Core Engine Components
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

// Enhanced Input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// DataTable and AnimInstance
#include "Engine/DataTable.h"
#include "PS_CharacterStats.h"
#include "PS_AnimInstance.h"

// Weapon
#include "PS_BasePickup.h"
#include "PS_Weapon.h"

// Attack
#include "PS_AnimInstance.h"
#include "Net/UnrealNetwork.h"

// Damage
#include "Engine/DamageEvents.h"

APS_Character::APS_Character()
{
	PrimaryActorTick.bCanEverTick = true;

	// Setup SpringArm Component
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 800.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Setup Camera Component
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Controller의 회전을 Character의 회전에 사용할지 유무
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Setup Capsule Component
	GetCapsuleComponent()->InitCapsuleSize(45.0f, 100.0f);

	// Setup Mesh Component
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("/Game/Characters/Adventurers/Knight/Knight"));
	if (SkeletalMeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SkeletalMeshAsset.Object);
	}

	// Setup CharacterMovement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 0.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;
	GetCharacterMovement()->GravityScale = 4.0f;
	GetCharacterMovement()->JumpZVelocity = 900.0f;
	JumpMaxHoldTime = 0.1f;
	JumpMaxCount = 1;

	// Setup Weapon variables
	CurrentHand = EHand::Bare_Handed;

	// Setup Attack variables
	bIsAttacking = false;
	MaxCombo = 2;
	AttackEndComboState();

	// Replication
	bReplicates = true;
}

void APS_Character::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// 캐릭터 Stat 초기화
	UpdateCharacterStats();
}

void APS_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 캐릭터가 공중에 떠 있으면 CharacterMovement의 RotationRate를 줄임
	GetCharacterMovement()->RotationRate = (GetMovementComponent()->IsFalling() ? FRotator(0.0f, 150.0f, 0.0f) : FRotator(0.0f, 500.0f, 0.0f));
}

void APS_Character::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Setup Delegates
	PS_AnimInstance = Cast<UPS_AnimInstance>(GetMesh()->GetAnimInstance());
	PS_CHECK(PS_AnimInstance != nullptr);

	PS_AnimInstance->OnMontageEnded.AddDynamic(this, &APS_Character::OnMontageEnded);
	PS_AnimInstance->OnNextAttackCheck.AddLambda([this]() -> void {
		if (GetLocalRole() == ROLE_Authority)
		{
			if (IsLocallyControlled())
			{
				// 서버에서 호출
				bCanNextCombo = false;

				if (bIsComboInputOn)
				{
					AttackStartComboState();
					JumpToMontageSection(PS_AnimInstance->AttackMontage, CurrentCombo);
				}
			}
			else
			{
				// 클라이언트에서 호출
				bCanNextCombo = false;

				if (bIsComboInputOn)
				{
					AttackStartComboState();
					JumpToMontageSection(PS_AnimInstance->AttackMontage, CurrentCombo);
				}
			}
		}
		
	});
}

void APS_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/*
	// Setup Replication variables
	DOREPLIFETIME_CONDITION(APS_Character, bIsAttacking, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APS_Character, bCanNextCombo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APS_Character, bIsComboInputOn, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APS_Character, CurrentCombo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APS_Character, MaxCombo, COND_OwnerOnly);
	*/
}

void APS_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 특정 액션에 함수 바인딩
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APS_Character::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APS_Character::Look);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APS_Character::Interact);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &APS_Character::SprintStart);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APS_Character::SprintEnd);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APS_Character::JumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APS_Character::JumpEnd);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APS_Character::Attack);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &APS_Character::Dodge);
	}
}

void APS_Character::UpdateCharacterStats()
{
	if (CharacterDataTable)
	{
		TArray<FPS_CharacterStats*> CharacterStatsRows;
		CharacterDataTable->GetAllRows<FPS_CharacterStats>(TEXT("PS_Character"), CharacterStatsRows);

		if (CharacterStatsRows.Num() > 0)
		{
			//const auto NewCharacterLevel = FMath::Clamp(CharacterLevel, 1, CharacterStatsRows.Num());
			//CharacterStats = CharacterStatsRows[NewCharacterLevel - 1];
			CharacterStats = CharacterStatsRows[0];

			GetCharacterMovement()->MaxWalkSpeed = GetCharacterStats()->WalkSpeed;
		}
	}
}

void APS_Character::Move(const FInputActionValue& Value)
{
	//if (!PS_AnimInstance->Montage_IsPlaying(NULL))
	if (!bIsAttacking)
	{
		const auto MovementVector = Value.Get<FVector2D>();

		if (Controller != nullptr)
		{
			const auto Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			const auto ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			const auto RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);
		}
	}
}

void APS_Character::Look(const FInputActionValue& Value)
{
	const auto LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APS_Character::SprintStart(const FInputActionValue& Value)
{
	// 인스턴스에서 호출
	SprintStart_Server();
}

void APS_Character::SprintStart_Server_Implementation()
{
	// 서버에서 호출
	SprintStart_Client();
}

void APS_Character::SprintStart_Client_Implementation()
{
	// 인스턴스마다 실행
	if (GetCharacterStats())
	{
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterStats()->SprintSpeed;
	}
}

void APS_Character::SprintEnd(const FInputActionValue& Value)
{
	// 인스턴스에서 호출
	SprintEnd_Server();
}

void APS_Character::SprintEnd_Server_Implementation()
{
	// 서버에서 호출
	SprintEnd_Client();
}

void APS_Character::SprintEnd_Client_Implementation()
{
	// 인스턴스마다 실행
	if (GetCharacterStats())
	{
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterStats()->WalkSpeed;
	}
}

void APS_Character::Interact(const FInputActionValue& Value)
{
	// 아직 미구현
	GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::Red, TEXT("Interact"));
}

void APS_Character::JumpStart(const FInputActionValue& Value)
{
	bPressedJump = true;
}

void APS_Character::JumpEnd(const FInputActionValue& Value)
{
	bPressedJump = false;
}

void APS_Character::Attack(const FInputActionValue& Value)
{
	if (HasAuthority())
	{
		// 서버에서 호출
		HandleAttack();
	}
	else
	{
		// 클라이언트에서 호출
		Attack_Server();
	}
}

bool APS_Character::Attack_Server_Validate()
{
	return true;
}

void APS_Character::Attack_Server_Implementation()
{
	// 서버에서 호출
	HandleAttack();
}

// 공격을 처리하는 함수 (트레이스와 데미지 계산)
void APS_Character::HandleAttack()
{
	if (bIsAttacking)
	{
		// 공격 시작
		PS_CHECK(FMath::IsWithinInclusive<int>(CurrentCombo, 1, MaxCombo));
		if (bCanNextCombo)
		{
			bIsComboInputOn = true;
		}
	}
	else
	{
		if (HasAuthority())
		{
			// 콤보 공격 시작
			PS_CHECK(CurrentCombo == 0);
			bIsAttacking = true;
			AttackStartComboState();
			PlayMontage(PS_AnimInstance->AttackMontage);
			PS_AnimInstance->JumpToMontageSection(PS_AnimInstance->AttackMontage, CurrentCombo);
		}
	}

	FRotator ControlRotation = GetControlRotation();
	FVector Start = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();
	FVector End = Start + (ForwardVector * AttackRange);

	FHitResult HitResult;
	FCollisionQueryParams TraceParams(FName(TEXT("AttackTrace")), true, this);
	TraceParams.bReturnPhysicalMaterial = false;

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECollisionChannel::ECC_Pawn,
		TraceParams
	);

	FColor DrawColor = bHit ? FColor::Green : FColor::Red;
	DrawDebugLine(GetWorld(), Start, End, DrawColor, false, 1, 0, 5);

	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Hit: %s"), *HitActor->GetName()));

			FDamageEvent DamageEvent;
			HitActor->TakeDamage(50.0f, DamageEvent, GetController(), this);
		}
	}

	// 공격 종료 처리
	//FTimerHandle UnusedHandle;
	//GetWorldTimerManager().SetTimer(UnusedHandle, this, &APS_Character::EndAttack, AttackDuration, false);
}

void APS_Character::Dodge(const FInputActionValue& Value)
{
	PS_LOG_S(Log);
	PlayMontage(PS_AnimInstance->DodgeMontage);
}

void APS_Character::PlayMontage(UAnimMontage* Montage)
{
	FString AnimType = PS_AnimInstance->MontageToString(Montage);
	UE_LOG(Project_S, Log, TEXT("AnimType : %s"), *AnimType);

	if (AnimType.Equals("Attack"))
	{
		// Attack Montage
		PlayMontage_Client(Montage);
	}
	else if (AnimType.Equals("Dodge"))
	{
		// Dodge Montage
		PlayMontage_Client(Montage);
		//Montage_JumpToSection(GetMontageSectionName(Montage, NewSection), Montage);
	}
	/*
	if (GetLocalRole() == ROLE_Authority)
	{
		if (IsLocallyControlled())
		{
			// 서버에서 호출
			PS_AnimInstance->Montage_Play(Montage, 1.0f);
		}
		else
		{
			// 클라이언트에서 호출
			PlayMontage_Client(Montage);
		}
	}
	*/
	PlayMontage_Client(Montage);
}

void APS_Character::PlayMontage_Client_Implementation(UAnimMontage* Montage)
{
	// 인스턴스마다 실행
	PS_AnimInstance->Montage_Play(Montage, 1.0f);
}

void APS_Character::JumpToMontageSection(UAnimMontage* Montage, int NewSection)
{
	/*
	if (GetLocalRole() == ROLE_Authority)
	{
		if (IsLocallyControlled())
		{
			// 서버에서 호출
			PS_AnimInstance->JumpToMontageSection(Montage, NewSection);
		}
		else
		{
			// 클라이언트에서 호출
			JumpToMontageSection_Client(Montage, NewSection);
		}
	}
	*/
	JumpToMontageSection_Client(Montage, NewSection);
}

void APS_Character::JumpToMontageSection_Client_Implementation(UAnimMontage* Montage, int NewSection)
{
	// 인스턴스마다 실행
	PS_AnimInstance->JumpToMontageSection(Montage, NewSection);
}

void APS_Character::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// Attack Montage
	if (GetLocalRole() == ROLE_Authority)
	{
		if (IsLocallyControlled())
		{
			// 서버에서 호출
			PS_CHECK(bIsAttacking);
			PS_CHECK(CurrentCombo > 0);
			bIsAttacking = false;
			AttackEndComboState();
		}
		else
		{
			// 클라이언트에서 호출
			//OnMontageEnded_Client(Montage, bInterrupted);
			PS_CHECK(bIsAttacking);
			PS_CHECK(CurrentCombo > 0);
			bIsAttacking = false;
			AttackEndComboState();
		}
	}
}

void APS_Character::OnMontageEnded_Client_Implementation(UAnimMontage* Montage, bool bInterrupted)
{
	// Attack Montage
	// 인스턴스마다 실행
	PS_CHECK(bIsAttacking);
	PS_CHECK(CurrentCombo > 0);
	bIsAttacking = false;
	AttackEndComboState();
}

void APS_Character::AttackStartComboState()
{
	// 콤보 공격 시작
	bCanNextCombo = true;
	bIsComboInputOn = false;
	PS_CHECK(FMath::IsWithinInclusive<int>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int>(CurrentCombo + 1, 1, MaxCombo);
}
void APS_Character::AttackEndComboState()
{
	// 콤보 공격 종료
	bIsComboInputOn = false;
	bCanNextCombo = false;
	CurrentCombo = 0;
}

float APS_Character::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	UE_LOG(LogTemp, Log, TEXT("Actor : %s took Damage : %f from %s"), *GetName(), FinalDamage, EventInstigator->GetFName());
	return FinalDamage;
}

bool APS_Character::CanSetWeapon(EHand Hand)
{
	switch (Hand)
	{
	case EHand::Bare_Handed:
		{
			return true;
		}
	case EHand::Left_Handed:
		{
			return true;
		}
	case EHand::Right_Handed:
		{
			return true;
		}
	case EHand::Two_Handed:
		{
			return true;
		}
	}
	return false;
}

void APS_Character::SetWeapon(APS_Weapon* NewWeapon, EHand NewHand)
{
	if (NewWeapon != nullptr)
	{
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, false);

		switch (NewHand)
		{
		case EHand::Bare_Handed:
			{
				if (CurrentLeftWeapon != nullptr)
				{
					CurrentLeftWeapon->Destroy();
				}
				if (CurrentRightWeapon != nullptr)
				{
					CurrentRightWeapon->Destroy();
				}
				CurrentLeftWeapon = nullptr;
				CurrentRightWeapon = nullptr;
				CurrentHand = EHand::Bare_Handed;
				break;
			}
		case EHand::Left_Handed:
			{
				if (CurrentHand == EHand::Left_Handed)
				{
					CurrentLeftWeapon->Destroy();
					CurrentLeftWeapon = nullptr;
				}
				else if (CurrentHand == EHand::Two_Handed)
				{
					CurrentRightWeapon->Destroy();
					CurrentLeftWeapon = nullptr;
					CurrentRightWeapon = nullptr;
				}

				FName WeaponLeftSocket(TEXT("hand_lSocket"));
				NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponLeftSocket);
				NewWeapon->SetOwner(this);
				CurrentLeftWeapon = NewWeapon;
				CurrentHand = EHand::Left_Handed;
				break;
			}
		case EHand::Right_Handed:
			{
				if (CurrentHand == EHand::Right_Handed)
				{
					CurrentRightWeapon->Destroy();
					CurrentRightWeapon = nullptr;
				}
				else if (CurrentHand == EHand::Two_Handed)
				{
					CurrentRightWeapon->Destroy();
					CurrentLeftWeapon = nullptr;
					CurrentRightWeapon = nullptr;
				}

				FName WeaponRightSocket(TEXT("hand_rSocket"));
				NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponRightSocket);
				NewWeapon->SetOwner(this);
				CurrentRightWeapon = NewWeapon;
				CurrentHand = EHand::Right_Handed;
				break;
			}
		case EHand::Two_Handed:
			{
				if (CurrentHand == EHand::Left_Handed)
				{
					CurrentLeftWeapon->Destroy();
					CurrentLeftWeapon = nullptr;
				}
				else if (CurrentHand == EHand::Right_Handed)
				{
					CurrentRightWeapon->Destroy();
					CurrentRightWeapon = nullptr;
				}
				else if (CurrentHand == EHand::Two_Handed)
				{
					CurrentRightWeapon->Destroy();
					CurrentLeftWeapon = nullptr;
					CurrentRightWeapon = nullptr;
				}

				FName WeaponRightSocket(TEXT("hand_rSocket"));
				NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponRightSocket);
				NewWeapon->SetOwner(this);
				CurrentLeftWeapon = NewWeapon;
				CurrentRightWeapon = NewWeapon;
				CurrentHand = EHand::Two_Handed;
				break;
			}
		}
	}
}