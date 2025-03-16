#pragma once

#include "Project_S.h"
#include "Engine/DataTable.h"
#include "PS_CharacterStats.generated.h"

USTRUCT(BlueprintType)
struct PROJECT_S_API FPS_CharacterStats : public FTableRowBase
{
	GENERATED_BODY()

	// ü��
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Hp;

	// ���ݷ�
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int AttackDamage;

	// ���� �ӵ�
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float AttackSpeed;

	// ����
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Armor;

	// ��� ����
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int ArmorPenetration;

	// ũ��Ƽ�� Ȯ��
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CriticalProbability;

	// ũ��Ƽ�� ����
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CriticalDamage;

	// �̵� �ӵ�
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float WalkSpeed;
	
	// �޸��� �ӵ�
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float SprintSpeed;
};