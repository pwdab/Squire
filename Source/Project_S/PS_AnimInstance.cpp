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
	IsSprinting = false;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("/Game/Blueprints/BP_Character_AnimMontage.BP_Character_AnimMontage"));
	if (ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}
}

// BeginPlay()와 유사
void UPS_AnimInstance::NativeInitializeAnimation()
{

}

// Tick()과 유사
void UPS_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn))
	{
		CurrentPawnSpeed = FVector2D(Pawn->GetVelocity().X, Pawn->GetVelocity().Y);
		auto Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			IsInAir = Character->GetMovementComponent()->IsFalling();
			MaxWalkSpeed = Character->GetCharacterMovement()->MaxWalkSpeed;
		}
	}
}
/*
void UPS_AnimInstance::PlayAttackMontage()
{
	
	auto Controller = TryGetPawnOwner()->GetController();
	if (::IsValid(Controller))
	{		
		if (Controller->GetLocalRole() == ROLE_Authority && Controller->GetRemoteRole() == ROLE_SimulatedProxy)  // 서버에서 바로 처리
		{
			PS_LOG_S(Log);
			PlayAttackMontage_Client();
		}
		else  // 클라이언트에서 호출한 경우 서버로 요청 전송
		{
			PS_LOG_S(Log);
			PlayAttackMontage_Server();
		}
	}
}

void UPS_AnimInstance::PlayAttackMontage_Server_Implementation()
{
	PS_LOG_S(Log);
	PlayAttackMontage_Client();
}

void UPS_AnimInstance::PlayAttackMontage_Client_Implementation()
{
	PS_LOG_S(Log);
	//Cast<ACharacter>(TryGetPawnOwner())->GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage, 1.0f);
	Montage_Play(AttackMontage, 1.0f);
}
*/

void UPS_AnimInstance::JumpToAttackMontageSection(int NewSection)
{
	PS_CHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
}

void UPS_AnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void UPS_AnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

FName UPS_AnimInstance::GetAttackMontageSectionName(int Section)
{
	PS_CHECK(FMath::IsWithinInclusive<int>(Section, 1, 2), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}