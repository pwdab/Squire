// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PS_Weapon.generated.h"
/*
UENUM(BlueprintType)
enum class EHand : uint8
{
	Left = 0 UMETA(DisplayName = "Left"),
	Right = 1 UMETA(DisplayName = "Right"),
	Two_Handed = 2 UMETA(DisplayName = "Two_Handed")
};
*/

UCLASS()
class PROJECT_S_API APS_Weapon : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> Mesh;

	/*
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	EHand Hand;
	*/

public:	
	// Sets default values for this actor's properties
	APS_Weapon();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*
	// Getter functions
	FORCEINLINE EHand GetHand() const { return Hand; }
	*/
};
