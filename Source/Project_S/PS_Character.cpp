// Fill out your copyright notice in the Description page of Project Settings.

#include "PS_Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PS_CharacterStats.h"
#include "Engine/DataTable.h"


// Sets default values
APS_Character::APS_Character()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 800.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCapsuleComponent()->InitCapsuleSize(45.0f, 100.0f);

	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("/Game/Characters/Knight/Knight"));
	if (SkeletalMeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SkeletalMeshAsset.Object);
	}

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;


	GetCharacterMovement()->GravityScale = 4.0f;
	GetCharacterMovement()->JumpZVelocity = 900.0f;
	JumpMaxHoldTime = 0.1f;
	JumpMaxCount = 1;
}

// Called when the game starts or when spawned
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

	UpdateCharacterStats();
}

// Called every frame
void APS_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::White, FString::Printf(TEXT("Name:\t%s"), this->get);
	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::White, FString::Printf(TEXT("Pos:\t%f %f %f"), GetTransform().GetLocation().X, GetTransform().GetLocation().X, GetTransform().GetLocation().X));
	GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::White, FString::Printf(TEXT("ANG:\t%f %f %f"), GetControlRotation().Yaw, GetControlRotation().Pitch, GetControlRotation().Roll));
	GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::White, FString::Printf(TEXT("VEL:\t%f"), FVector(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y, 0.0f).Length()));
	

	if (GetMovementComponent()->IsFalling())
	{
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 150.0f, 0.0f);
	}
	else
	{
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	}
}

// Called to bind functionality to input
void APS_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


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
	//GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Yellow, FString::Printf(TEXT("MovementVector: %s"), *MovementVector.ToString()));

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
	//GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("LookAxisVector: %s"), *LookAxisVector.ToString()));

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APS_Character::SprintStart(const FInputActionValue& Value)
{
	/*
	GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Blue, TEXT("SprintStart"));
	GetCharacterMovement()->MaxWalkSpeed = 1000.f;
	*/
	SprintStart_Server();
}

void APS_Character::SprintEnd(const FInputActionValue& Value)
{
	/*
	GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Blue, TEXT("SprintEnd"));
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	*/
	SprintEnd_Server();
}

void APS_Character::SprintStart_Server_Implementation()
{
	//GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Blue, TEXT("SprintStart"));
	//GetCharacterMovement()->MaxWalkSpeed = 1000.f;
	if (GetCharacterStats())
	{
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterStats()->SprintSpeed;
	}
}

void APS_Character::SprintEnd_Server_Implementation()
{
	//GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Blue, TEXT("SprintEnd"));
	//GetCharacterMovement()->MaxWalkSpeed = 500.f;
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
	//GEngine->AddOnScreenDebugMessage(4, 5.f, FColor::Green, TEXT("JumpStart"));
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 50.0f, 0.0f);
	bPressedJump = true;
}

void APS_Character::JumpEnd(const FInputActionValue& Value)
{
	//GEngine->AddOnScreenDebugMessage(4, 5.f, FColor::Green, TEXT("JumpEnd"));
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	bPressedJump = false;
}

void APS_Character::Attack(const FInputActionValue& Value)
{
	//Attack
}
