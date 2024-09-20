// Fill out your copyright notice in the Description page of Project Settings.

#include "PS_AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PS_Character.h"

UPS_AnimInstance::UPS_AnimInstance()
{
	Walk_MaxWalkSpeed = 500.0f;
	CurrentPawnSpeed = FVector2D::ZeroVector;
	IsInAir = false;
}

void UPS_AnimInstance::NativeInitializeAnimation()
{

}

void UPS_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn))
	{
		CurrentPawnSpeed = FVector2D(Pawn->GetVelocity().X, Pawn->GetVelocity().Y);
		auto Character = Cast<ACharacter>(Pawn);
		auto PS_Character = Cast<APS_Character>(Character);
		if (Character)
		{
			IsInAir = Character->GetMovementComponent()->IsFalling();
		}
		
		if (PS_Character->bIsAttacking)  // bIsAttacking은 가상의 변수로, 실제로는 캐릭터 클래스의 상태를 확인하는 로직이 필요
		{
			IsAttacking = true;
		}
		else
		{
			IsAttacking = false;
		}

	}
}

