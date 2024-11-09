// Fill out your copyright notice in the Description page of Project Settings.

#include "PS_Character.h"

// Core Engine Components
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimeLineComponent.h"
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

// Interact
#include "PS_Interactable.h"
#include "PS_Grabable.h"
#include "Kismet/KismetSystemLibrary.h"

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
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("PS_Character"));

	// Setup Mesh Component
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("/Game/Characters/Adventurers/Knight/Knight"));
	if (SkeletalMeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SkeletalMeshAsset.Object);
	}

	// Setup CharacterMovement
	GetCharacterMovement()->bOrientRotationToMovement = false;
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

	// Setup Dodge variables
	Dodge_Direction = EDirection::N;
	bIsDodging = false;

	// Interact variables
	InteractableActor = nullptr;
	GrabableActor = nullptr;
	GrabbedActor = nullptr;

	// Actor Turn variables
	bIsMoving = false;
	bIsTurning = false;
	HeadRotator = FRotator::ZeroRotator;

	// Material variables
	DitherAlpha = 1.0f;

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

	// Interact
	if (GetLocalRole() != ROLE_Authority) return;
	else
	{
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.bTraceComplex = true;
		QueryParams.AddIgnoredActor(this);

		auto SphereRadius = 50.0f;
		auto StartLocation = GetActorLocation() + FVector(0.0f, 0.0f, 25.0f);
		FVector ViewVector = FVector(FMath::Cos(FMath::DegreesToRadians(GetControlRotation().Yaw)), FMath::Sin(FMath::DegreesToRadians(GetControlRotation().Yaw)), FMath::Tan(FMath::DegreesToRadians(GetControlRotation().Pitch)));
		auto EndLocation = StartLocation + ViewVector * 250.0f;

		DrawDebugDirectionalArrow(GetWorld(), StartLocation, EndLocation, 150.0f, FColor::Yellow, false, -1.0f, 0, 5.0f);
		//auto IsHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), StartLocation, EndLocation, SphereRadius, UEngineTypes::ConvertToTraceType(ECC_WorldStatic), false, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, HitResult, true);
		auto IsHit = GetWorld()->SweepSingleByChannel(HitResult, StartLocation, EndLocation, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel2, FCollisionShape::MakeSphere(SphereRadius), QueryParams);
		if (!GrabbedActor && IsHit && HitResult.GetActor()->GetClass()->ImplementsInterface(UPS_Grabable::StaticClass()) && HitResult.GetActor()->GetOwner() == nullptr)
		{
			DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, SphereRadius, 12, FColor::Yellow, false, 1.0f);
			GrabableActor = HitResult.GetActor();
		}
		else
		{
			GrabableActor = nullptr;
		}

		IsHit = GetWorld()->SweepSingleByChannel(HitResult, StartLocation, EndLocation, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel5, FCollisionShape::MakeSphere(SphereRadius), QueryParams);
		if (IsHit && HitResult.GetActor()->GetClass()->ImplementsInterface(UPS_Interactable::StaticClass()))
		{
			DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, SphereRadius, 12, FColor::Magenta, false, 1.0f);
			InteractableActor = HitResult.GetActor();
		}
		else
		{
			InteractableActor = nullptr;
		}
	}
	

	// Turn
	if (!bIsMoving)
	{
		float ControlYaw = GetControlRotation().Yaw;
		float ActorYaw = GetActorRotation().Yaw;

		if (ActorYaw < 0)
		{
			ActorYaw += 360.0f;
		}

		// delta_angle이 70도가 될 때까지 캐릭터의 몸체는 움직이지 않음
		float delta_angle = ControlYaw - ActorYaw;

		if (delta_angle < 0)
		{
			delta_angle += 360.0f;
		}

		if (delta_angle > 180)
		{
			delta_angle -= 360.0f;
		}

		// delta_angle이 70도보다 커지면 캐릭터의 몸체도 같이 움직여 delta_angle이 70도보다 커지는 것을 방지
		// 오른쪽으로 회전
		if (delta_angle > 70)
		{
			ActorYaw += delta_angle - 70.0f;
			FRotator new_rotator = GetActorRotation();
			new_rotator.Yaw = ActorYaw;
			//SetActorRotation(new_rotator);
			RotateActor(new_rotator);
			bIsTurning = true;
		}
		// 왼쪽으로 회전
		else if (delta_angle < -70)
		{
			ActorYaw += delta_angle + 70.0f;
			FRotator new_rotator = GetActorRotation();
			new_rotator.Yaw = ActorYaw;
			//SetActorRotation(new_rotator);
			RotateActor(new_rotator);
			bIsTurning = true;
		}

		// delta_angle이 70도보다 커지면 캐릭터의 몸체를 카메라 방향으로 천천히 돌린다
		if (FMath::Abs(ControlYaw - ActorYaw) <= 10.0f)
		{
			bIsTurning = false;
		}
		else
		{
			RotateActor(FMath::RInterpTo(GetActorRotation(), FRotator(GetActorRotation().Pitch, Controller->GetControlRotation().Yaw, GetActorRotation().Roll), GetWorld()->GetDeltaSeconds(), 5.0f));
		}
	}
	
	// SpringArm의 길이가 일정 이하로 줄어들면 Mesh를 투명하게 만듦
	float CurrentCameraDistance = (GetFollowCamera()->GetComponentLocation() - GetActorLocation()).Size();
	if (CurrentCameraDistance < 200.0f)
	{
		// Opacity를 길이에 비례하여 감소
		DitherAlpha = FMath::Clamp((CurrentCameraDistance - 100.0f) / 200.0f, 0.0f, 1.0f);
	}
	else
	{
		DitherAlpha = 1.0f;
	}

	// 캐릭터가 공중에 떠 있으면 CharacterMovement의 RotationRate를 줄임
	GetCharacterMovement()->RotationRate = (GetMovementComponent()->IsFalling() ? FRotator(0.0f, 150.0f, 0.0f) : FRotator(0.0f, 500.0f, 0.0f));
}

void APS_Character::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//if (GetLocalRole() != ROLE_Authority) return;

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

	// Setup Replication variables
	//DOREPLIFETIME_CONDITION(APS_Character, Dodge_Direction, COND_OwnerOnly);
	/*
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
		EnhancedInputComponent->BindAction(MoveEndAction, ETriggerEvent::Completed, this, &APS_Character::MoveEnd);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APS_Character::Look);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APS_Character::Interact);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &APS_Character::SprintStart);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APS_Character::SprintEnd);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APS_Character::JumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APS_Character::JumpEnd);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APS_Character::Attack);
		EnhancedInputComponent->BindAction(DodgeDirectionStartAction, ETriggerEvent::Triggered, this, &APS_Character::DodgeDirectionStart);
		EnhancedInputComponent->BindAction(DodgeDirectionEndAction, ETriggerEvent::Completed, this, &APS_Character::DodgeDirectionEnd);
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
	bIsMoving = true;

	if (!PS_AnimInstance->Montage_IsPlaying(NULL))
	//if (!bIsAttacking)
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

			// YawRotation 방향과 MovementVector 사이의 내적값
			float DotProduct = FMath::Clamp(FVector2D::DotProduct(FVector2D(0.0f, 1.0f), MovementVector.GetSafeNormal()), -1.0f, 1.0f);

			// 내적값을 통해 각도로 변환
			float AngleInDegrees = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
			// 왼쪽으로 가는 경우
			if (MovementVector.X < 0)
			{
				AngleInDegrees *= -1;
			}

			// 앞으로 가는 경우
			if (FMath::Abs(AngleInDegrees) <= 90)
			{
				AngleInDegrees = FMath::Clamp(AngleInDegrees, -71.0f, 71.0f);		// 71도로 설정해야 멈췄을 때 Look()에서 정면을 바라보도록 할 수 있음
				
			}
			// 뒤로 가는 경우
			else
			{
				AngleInDegrees = (AngleInDegrees < 0) ? (180 + AngleInDegrees) : (-180 + AngleInDegrees);
			}

			//SetActorRotation(FMath::RInterpTo(GetActorRotation(), FRotator(GetActorRotation().Pitch, Controller->GetControlRotation().Yaw + AngleInDegrees, GetActorRotation().Roll), GetWorld()->GetDeltaSeconds(), 10.0f));
			RotateActor(FMath::RInterpTo(GetActorRotation(), FRotator(GetActorRotation().Pitch, Controller->GetControlRotation().Yaw + AngleInDegrees, GetActorRotation().Roll), GetWorld()->GetDeltaSeconds(), 5.0f));
			//SetActorRotation(FRotator(GetActorRotation().Pitch, Controller->GetControlRotation().Yaw + AngleInDegrees, GetActorRotation().Roll));
		}
	}
}

void APS_Character::MoveEnd(const FInputActionValue& Value)
{
	bIsMoving = false;
}

void APS_Character::Look(const FInputActionValue& Value)
{
	const auto LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);

		HeadRotator.Roll = -GetControlRotation().Pitch + 90.0f;
		if (HeadRotator.Roll < 0)
		{
			HeadRotator.Roll += 360.0f;
		}

		// 머리 위아래
		HeadRotator.Roll = FMath::Clamp(HeadRotator.Roll, 90 - MAX_ROTATION_ROLL, 90 - MIN_ROTATION_ROLL);
		// 머리 좌우
		HeadRotator.Yaw = GetControlRotation().Yaw - 90.0f - GetActorRotation().Yaw;

		SetHeadRotator(HeadRotator);
		//PS_AnimInstance->SetControlRotation(HeadRotator);
		//UE_LOG(Project_S, Log, TEXT("HeadRotator = %s\n"), *HeadRotator.ToString());
	}
}

void APS_Character::RotateActor(FRotator NewRotator)
{
	RotateActor_Server(NewRotator);
}

void APS_Character::RotateActor_Server_Implementation(FRotator NewRotator)
{
	RotateActor_Client(NewRotator);
}

void APS_Character::RotateActor_Client_Implementation(FRotator NewRotator)
{
	SetActorRotation(NewRotator);
}

void APS_Character::SetHeadRotator(FRotator NewRotator)
{
	SetHeadRotator_Server(NewRotator);
}

void APS_Character::SetHeadRotator_Server_Implementation(FRotator NewRotator)
{
	SetHeadRotator_Client(NewRotator);
}

void APS_Character::SetHeadRotator_Client_Implementation(FRotator NewRotator)
{
	PS_AnimInstance->SetControlRotation(NewRotator);
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
	//GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::Red, TEXT("Interact"));
	Interact_Server();
}

void APS_Character::Interact_Server_Implementation()
{
	if (InteractableActor)
	{
		IPS_Interactable::Execute_Interact(InteractableActor, this);
	}
	else
	{
		UE_LOG(Project_S, Log, TEXT("InteractableActor is null"));
	}

	if (GrabableActor)
	{
		if (!GrabbedActor)
		{
			IPS_Grabable::Execute_Grab(GrabableActor, this);
			GrabbedActor = GrabableActor;
		}
		else
		{
			UE_LOG(Project_S, Log, TEXT("GrabbedActor is not null"));
		}
	}
	else
	{
		if (GrabbedActor)
		{
			IPS_Grabable::Execute_UnGrab(GrabbedActor, this);
			GrabbedActor = nullptr;
		}
		else
		{
			UE_LOG(Project_S, Log, TEXT("GrabableActor is null"));
		}
	}
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

void APS_Character::DodgeDirectionStart(const FInputActionValue& Value)
{
	const auto MovementVector = Value.Get<FVector2D>();
	if (MovementVector.X > 0) {
		if (MovementVector.Y > 0) {
			Dodge_Direction = EDirection::WD;
		}
		else if (MovementVector.Y < 0) {
			Dodge_Direction = EDirection::SD;
		}
		else {
			Dodge_Direction = EDirection::D;
		}
	}
	else if (MovementVector.X < 0) {
		if (MovementVector.Y > 0) {
			Dodge_Direction = EDirection::WA;
		}
		else if (MovementVector.Y < 0) {
			Dodge_Direction = EDirection::SA;
		}
		else {
			Dodge_Direction = EDirection::A;
		}
	}
	else {
		if (MovementVector.Y > 0) {
			Dodge_Direction = EDirection::W;
		}
		else if (MovementVector.Y < 0) {
			Dodge_Direction = EDirection::S;
		}
		else {
			Dodge_Direction = EDirection::N;
		}
	}
	//UE_LOG(Project_S, Log, TEXT("Dodge_Direction = %d\n"), Dodge_Direction);
}

void APS_Character::DodgeDirectionEnd(const FInputActionValue& Value)
{
	Dodge_Direction = EDirection::N;
	//UE_LOG(Project_S, Log, TEXT("Dodge_Direction = %d\n"), Dodge_Direction);
}

void APS_Character::Dodge(const FInputActionValue& Value)
{
	if (!bIsDodging)
	{
		if (Controller != nullptr)
		{
			const auto Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			FVector LaunchVector;
			switch (Dodge_Direction)
			{
			case EDirection::N:
			case EDirection::W:
				LaunchVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
				break;
			case EDirection::WA:
				LaunchVector = (FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) - FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y)).GetSafeNormal();
				break;
			case EDirection::A:
				LaunchVector = -FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
				break;
			case EDirection::SA:
				LaunchVector = (-FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) - FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y)).GetSafeNormal();
				break;
			case EDirection::S:
				LaunchVector = -FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
				break;
			case EDirection::SD:
				LaunchVector = (-FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) + FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y)).GetSafeNormal();
				break;
			case EDirection::D:
				LaunchVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
				break;
			case EDirection::WD:
				LaunchVector = (FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) + FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y)).GetSafeNormal();
				break;
			}
			LaunchVector *= 250.0f;

			if (HasAuthority())
			{
				FColor DrawColor = FColor::Green;
				//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + (LaunchVector), DrawColor, false, 1, 0, 5);
				DrawDebugDirectionalArrow(GetWorld(), GetActorLocation() + FVector(0.f, 0.f, 100.0f), GetActorLocation() + LaunchVector + FVector(0.f, 0.f, 100.0f), 150.0f, DrawColor, false, 1.0f, 0, 5.0f);
				// 서버에서 호출
				Dodge_Client(LaunchVector);
			}
			else
			{
				// 클라이언트에서 호출
				Dodge_Server(LaunchVector);
			}
		}
	}
}

void APS_Character::Dodge_Server_Implementation(const FVector Value)
{
	Dodge_Client(Value);
}

void APS_Character::Dodge_Client_Implementation(const FVector Value)
{
	// Blueprint 이벤트 호출
	OnDodge(Value);
	//LaunchCharacter(Value * 5000.0f, false, true);
	SetActorRotation(Value.Rotation());
	PlayMontage(PS_AnimInstance->DodgeMontage);
}

void APS_Character::PlayMontage(UAnimMontage* Montage)
{
	//if (AnimType.Equals("Attack"))
	if (Montage->GetPathName().Equals(PS_AnimInstance->AttackMontage->GetPathName()))
	{
		// Attack Montage
		PlayMontage_Client(Montage);
	}
	//else if (AnimType.Equals("Dodge"))
	else if (Montage->GetPathName().Equals(PS_AnimInstance->DodgeMontage->GetPathName()))
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
	//PlayMontage_Client(Montage);
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
	if (Montage->GetPathName().Equals(PS_AnimInstance->AttackMontage->GetPathName()))
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
	else if (Montage->GetPathName().Equals(PS_AnimInstance->DodgeMontage->GetPathName()))
	{

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