// Fill out your copyright notice in the Description page of Project Settings.

#include "PS_AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UPS_AnimInstance::UPS_AnimInstance()
{
	MaxWalkSpeed = 0.0f;
	CurrentPawnSpeed = FVector2D::ZeroVector;
	IsInAir = false;

	LoadAnimMontage(AttackMontage, TEXT("/Game/Blueprints/BP_Character_Attack_AnimMontage.BP_Character_Attack_AnimMontage"));
	LoadAnimMontage(DodgeMontage, TEXT("/Game/Blueprints/BP_Character_Dodge_AnimMontage.BP_Character_Dodge_AnimMontage"));
}

// BeginPlay()와 유사
void UPS_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

// Tick()과 유사
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
		}
	}
}

void UPS_AnimInstance::JumpToMontageSection(UAnimMontage* Montage, int NewSection)
{
	PS_CHECK(Montage_IsPlaying(Montage));
	Montage_JumpToSection(GetMontageSectionName(Montage, NewSection), Montage);
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
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageFinder(Path);
	if (MontageFinder.Succeeded())
	{
		Montage = MontageFinder.Object;
	}
}

FName UPS_AnimInstance::GetMontageSectionName(UAnimMontage* Montage, int Section)
{
	// Attack Montage
	PS_CHECK(FMath::IsWithinInclusive<int>(Section, 1, 2), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}