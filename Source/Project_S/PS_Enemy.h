// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Project_S.h"
#include "GameFramework/Character.h"
#include "PS_Enemy.generated.h"

UCLASS()
class PROJECT_S_API APS_Enemy : public ACharacter
{
	GENERATED_BODY()

public:
	APS_Enemy();

	// Override functions
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// User functions
	void UpdateEnemyStats();	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Data table reference variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	class UDataTable* EnemyDataTable;

	// PS_CharacterStats pointer variables
	struct FPS_CharacterStats* EnemyStats;
};
