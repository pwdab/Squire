// Fill out your copyright notice in the Description page of Project Settings.

#include "PS_AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PS_Character.h"

UPS_AnimInstance::UPS_AnimInstance()
{
	MaxWalkSpeed = 0.0f;
	CurrentPawnSpeed = FVector2D::ZeroVector;
	IsInAir = false;

	LoadAnimMontage(AttackMontage, TEXT("/Game/Blueprints/BP_Character_Attack_AnimMontage.BP_Character_Attack_AnimMontage"));
	LoadAnimMontage(DodgeMontage, TEXT("/Game/Blueprints/BP_Character_Dodge_AnimMontage.BP_Character_Dodge_AnimMontage"));

	ControlRotation.Roll = 90.0f;
	ControlRotation.Yaw = -90.0f;
}

// BeginPlay()�� ����
void UPS_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

// Tick()�� ����
void UPS_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (auto Pawn = TryGetPawnOwner())
	{
		CurrentPawnSpeed = FVector2D(Pawn->GetVelocity().X, Pawn->GetVelocity().Y);
		if (auto Character = Cast<ACharacter>(Pawn))
		{
			IsInAir = Character->GetMovementComponent()->IsFalling();
			MaxWalkSpeed = Character->GetCharacterMovement()->MaxWalkSpeed;

			/*
			ControlRotation.Roll = -Character->GetControlRotation().Pitch + 90.0f;
			if (ControlRotation.Roll < 0)
			{
				ControlRotation.Roll += 360.0f;
			}
			// �Ӹ� ���Ʒ�
			ControlRotation.Roll = FMath::Clamp(ControlRotation.Roll, 90 - MAX_ROTATION_ROLL, 90 - MIN_ROTATION_ROLL);
			// �Ӹ� �¿�
			ControlRotation.Yaw = Character->GetControlRotation().Yaw - 90.0f - Character->GetActorRotation().Yaw;
			*/
			CurrentPawnDirection = FMath::Clamp(FVector::DotProduct(Character->GetActorForwardVector(), Character->GetVelocity()), -1, 1);
		}
	}
}

void UPS_AnimInstance::JumpToMontageSection(UAnimMontage* Montage, int NewSection)
{
	PS_CHECK(Montage_IsPlaying(Montage));
	Montage_JumpToSection(GetMontageSectionName(Montage, NewSection), Montage);
}

void UPS_AnimInstance::SetControlRotation(FRotator Rotator)
{
	ControlRotation = Rotator;
}

void UPS_AnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void UPS_AnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

void UPS_AnimInstance::LoadAnimMontage(UAnimMontage*& Montage, const TCHAR* Path)
{
	ConstructorHelpers::FObjectFinder<UAnimMontage> MontageFinder(Path);
	if (MontageFinder.Succeeded())
	{
		Montage = MontageFinder.Object;
	}
}

FName UPS_AnimInstance::GetMontageSectionName(UAnimMontage* Montage, int Section)
{

	//if (AnimType.Equals("Attack"))
	if(Montage->GetPathName().Equals(AttackMontage->GetPathName()))
	{
		// Attack Montage
		PS_CHECK(FMath::IsWithinInclusive<int>(Section, 1, 2), NAME_None);
		return FName(*FString::Printf(TEXT("Attack%d"), Section));
	}
	//else if (AnimType.Equals("Dodge"))
	else if (Montage->GetPathName().Equals(DodgeMontage->GetPathName()))
	{
		// Dodge Montage
		return FName(*FString::Printf(TEXT("Dodge%d"), Section));
	}
	
	return FName(*FString::Printf(TEXT(""), Section));
}