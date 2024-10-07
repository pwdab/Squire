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

// Damage
#include "Engine/DamageEvents.h"

// Sets default values
APS_Character::APS_Character()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// SpringArm 컴포넌트
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 800.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Camera 컴포넌트
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Controller의 회전을 Character의 회전에 사용할지 유무
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// CapsuleComponent 설정
	GetCapsuleComponent()->InitCapsuleSize(45.0f, 100.0f);

	// Mesh 설정
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("/Game/Characters/Adventurers/Knight/Knight"));
	if (SkeletalMeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SkeletalMeshAsset.Object);
	}

	// CharacterMovement 설정
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 0.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;
	GetCharacterMovement()->GravityScale = 4.0f;
	GetCharacterMovement()->JumpZVelocity = 900.0f;
	JumpMaxHoldTime = 0.1f;
	JumpMaxCount = 1;

	// Weapon 설정
	CurrentHand = EHand::Bare_Handed;
	//WeaponItemClass = APS_Weapon::StaticClass();
}

// Called when the game starts or when spawned
void APS_Character::BeginPlay()
{
	Super::BeginPlay();

	/*
	// Weapon 설정
	auto NewWeapon = GetWorld()->SpawnActor<APS_Weapon>(WeaponItemClass, FVector::ZeroVector, FRotator::ZeroRotator);
	SetWeapon(NewWeapon);
	*/

	/*
	FName WeaponLeftSocket(TEXT("hand_lSocket"));
	auto CurLeftWeapon = GetWorld()->SpawnActor<APS_Weapon>(FVector::ZeroVector, FRotator::ZeroRotator);
	if (CurLeftWeapon != nullptr)
	{
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, false);
		CurLeftWeapon->AttachToComponent(GetMesh(), AttachmentRules, WeaponLeftSocket);
	}

	FName WeaponRightSocket(TEXT("hand_rSocket"));
	auto CurRightWeapon = GetWorld()->SpawnActor<APS_Weapon>(FVector::ZeroVector, FRotator::ZeroRotator);
	if (CurRightWeapon != nullptr)
	{
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, false);
		CurRightWeapon->AttachToComponent(GetMesh(), AttachmentRules, WeaponRightSocket);
	}
	*/
	
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

// Called every frame
void APS_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 캐릭터가 공중에 떠 있으면 CharacterMovement의 RotationRate를 줄임
	GetCharacterMovement()->RotationRate = (GetMovementComponent()->IsFalling() ? FRotator(0.0f, 150.0f, 0.0f) : FRotator(0.0f, 500.0f, 0.0f));
}

void APS_Character::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 델리게이트 등록
	
}

// Called to bind functionality to input
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
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APS_Character::AttackStart);
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
	// 클라이언트에서 실행하는 함수
	SprintStart_Server();
}

void APS_Character::SprintStart_Server_Implementation()
{
	// 서버에서 호출하는 함수
	SprintStart_Client();
}

void APS_Character::SprintStart_Client_Implementation()
{
	// 서버에서 호출하고 각 인스턴스마다 실행하는 함수
	if (GetCharacterStats())
	{
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterStats()->SprintSpeed;
	}
}

void APS_Character::SprintEnd(const FInputActionValue& Value)
{
	// 클라이언트에서 실행하는 함수
	SprintEnd_Server();
}

void APS_Character::SprintEnd_Server_Implementation()
{
	// 서버에서 호출하는 함수
	SprintEnd_Client();
}

void APS_Character::SprintEnd_Client_Implementation()
{
	// 서버에서 호출하고 각 인스턴스마다 실행하는 함수
	if (GetCharacterStats())
	{
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterStats()->WalkSpeed;
	}
}

void APS_Character::Interact(const FInputActionValue& Value)
{
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



// AttackStart 함수를 서버로 보내기 위한 함수
void APS_Character::AttackStart(const FInputActionValue& Value)
{
	if (HasAuthority())  // 서버에서 바로 처리
	{
		HandleAttack();
	}
	else  // 클라이언트에서 호출한 경우 서버로 요청 전송
	{
		ServerAttack();
	}
}

// 서버에서 공격 처리하는 함수
void APS_Character::ServerAttack_Implementation()
{
	HandleAttack();
}

// 서버 호출을 선언
bool APS_Character::ServerAttack_Validate()
{
	return true;
}

// 공격을 처리하는 함수 (트레이스와 데미지 계산)
void APS_Character::HandleAttack()
{
	// 공격 중으로 설정
	bIsAttacking = true;

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
	FTimerHandle UnusedHandle;
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &APS_Character::EndAttack, AttackDuration, false);
}

void APS_Character::EndAttack()
{
	// 공격이 끝났음을 표시
	bIsAttacking = false;
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
				bool bTemp = NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponLeftSocket);
				UE_LOG(LogTemp, Warning, TEXT("Left Handed"));
				if (bTemp)
				{
					UE_LOG(LogTemp, Warning, TEXT("AttachToComponent is successful"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("AttachToComponent is un-successful"));
				}
				NewWeapon->SetOwner(this);
				CurrentLeftWeapon = NewWeapon;
				CurrentHand = EHand::Left_Handed;
				break;
			}
		case EHand::Right_Handed:
			{
				if (CurrentHand == EHand::Right_Handed)
				{
					bool bTemp = CurrentRightWeapon->Destroy();
					if (bTemp)
					{
						UE_LOG(LogTemp, Warning, TEXT("CurrentRightWeapon destroyed"));
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("CurrentRightWeapon is not destroyed"));
					}
					CurrentRightWeapon = nullptr;
				}
				else if (CurrentHand == EHand::Two_Handed)
				{
					bool bTemp = CurrentRightWeapon->Destroy();
					if (bTemp)
					{
						UE_LOG(LogTemp, Warning, TEXT("CurrentRightWeapon destroyed"));
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("CurrentRightWeapon is not destroyed"));
					}
					CurrentLeftWeapon = nullptr;
					CurrentRightWeapon = nullptr;
				}

				FName WeaponRightSocket(TEXT("hand_rSocket"));
				bool bTemp = NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponRightSocket);
				UE_LOG(LogTemp, Warning, TEXT("Right Handed"));
				if (bTemp)
				{
					UE_LOG(LogTemp, Warning, TEXT("AttachToComponent is successful"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("AttachToComponent is un-successful"));
				}
				NewWeapon->SetOwner(this);
				CurrentRightWeapon = NewWeapon;
				CurrentHand = EHand::Right_Handed;
				break;
			}
		case EHand::Two_Handed:
			{
				if (CurrentHand == EHand::Left_Handed)
				{
					bool bTemp = CurrentLeftWeapon->Destroy();
					if (bTemp)
					{
						UE_LOG(LogTemp, Warning, TEXT("CurrentLeftWeapon destroyed"));
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("CurrentLeftWeapon is not destroyed"));
					}
					CurrentLeftWeapon = nullptr;
				}
				else if (CurrentHand == EHand::Right_Handed)
				{
					bool bTemp = CurrentRightWeapon->Destroy();
					if (bTemp)
					{
						UE_LOG(LogTemp, Warning, TEXT("CurrentRightWeapon destroyed"));
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("CurrentRightWeapon is not destroyed"));
					}
					CurrentRightWeapon = nullptr;
				}
				else if (CurrentHand == EHand::Two_Handed)
				{
					bool bTemp = CurrentRightWeapon->Destroy();
					if (bTemp)
					{
						UE_LOG(LogTemp, Warning, TEXT("CurrentRightWeapon destroyed"));
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("CurrentRightWeapon is not destroyed"));
					}
					CurrentLeftWeapon = nullptr;
					CurrentRightWeapon = nullptr;
				}

				FName WeaponRightSocket(TEXT("hand_rSocket"));
				bool bTemp = NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponRightSocket);
				UE_LOG(LogTemp, Warning, TEXT("Two Handed"));
				if (bTemp)
				{
					UE_LOG(LogTemp, Warning, TEXT("AttachToComponent is successful"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("AttachToComponent is un-successful"));
				}
				NewWeapon->SetOwner(this);
				CurrentLeftWeapon = NewWeapon;
				CurrentRightWeapon = NewWeapon;
				CurrentHand = EHand::Two_Handed;
				break;
			}
		}
	}
}