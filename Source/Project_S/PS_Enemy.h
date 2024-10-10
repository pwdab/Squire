// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Project_S.h"
#include "GameFramework/Character.h"
#include "PS_Enemy.generated.h"

UCLASS()
class PROJECT_S_API APS_Enemy : public ACharacter
{
	GENERATED_BODY()

	// DataTable 포인터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData, meta = (AllowPrivateAccess = "true"))
	class UDataTable* EnemyDataTable;

	// PS_CharacterStats 포인터
	struct FPS_CharacterStats* EnemyStats;

public:
	// Sets default values for this character's properties
	APS_Enemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Override functions
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// User functions
	void UpdateEnemyStats();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
