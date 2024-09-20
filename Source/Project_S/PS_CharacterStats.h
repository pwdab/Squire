#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PS_CharacterStats.generated.h"

USTRUCT(BlueprintType)
struct PROJECT_S_API FPS_CharacterStats : public FTableRowBase
{
	GENERATED_BODY()

	// 체력
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Hp;

	// 공격력
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int AttackDamage;

	// 공격 속도
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float AttackSpeed;

	// 방어력
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Armor;

	// 방어 관통
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int ArmorPenetration;

	// 크리티컬 확률
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CriticalProbability;

	// 크리티컬 배율
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CriticalDamage;

	// 이동 속도
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float WalkSpeed;
	
	// 달리기 속도
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float SprintSpeed;
};