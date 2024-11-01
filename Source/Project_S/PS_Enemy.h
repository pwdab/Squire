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
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// User functions
	void UpdateEnemyStats();

	UFUNCTION(BlueprintCallable, Category = "Enemy AI")
	void SetNextPatrolLocation();

	UFUNCTION(BlueprintCallable, Category = "Enemy AI")
	void Chase(APawn* Pawn);

	// Event handlers
	UFUNCTION()
	void OnPawnDetected(APawn* Pawn);

	UFUNCTION()
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// variables
	APawn* ChasingPawn;

	// AI variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy AI")
	float PatrolSpeed = 150.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy AI")
	float ChaseSpeed = 350.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy AI")
	float PatrolRadius = 1000.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Component variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPawnSensingComponent> PawnSense;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USphereComponent> Collision;

	UPROPERTY()
	FVector PatrolLocation;

	// Data table reference variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	class UDataTable* EnemyDataTable;

	// PS_CharacterStats pointer variables
	struct FPS_CharacterStats* EnemyStats;

public:
	// Getter functions
	FORCEINLINE UPawnSensingComponent* GetPawnSense() const { return PawnSense;	}
	FORCEINLINE USphereComponent* GetCollision() const { return Collision; }
};
