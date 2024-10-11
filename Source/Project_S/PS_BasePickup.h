// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Project_S.h"
#include "GameFramework/Actor.h"
#include "PS_BasePickup.generated.h"

UENUM(BlueprintType)
enum class EHand : uint8
{
	Bare_Handed UMETA(DisplayName = "Bare_Handed"),
	Left_Handed UMETA(DisplayName = "Left_Handed"),
	Right_Handed UMETA(DisplayName = "Right_Handed"),
	Two_Handed UMETA(DisplayName = "Two_Handed")
};

UCLASS()
class PROJECT_S_API APS_BasePickup : public AActor
{
	GENERATED_BODY()
	
public:
	APS_BasePickup();

	// Override functions
	virtual void Tick(float DeltaTime) override;

	// Weapon variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components)
	TSubclassOf<class APS_Weapon> WeaponItemClass;
	
protected:
	virtual void BeginPlay() override;

	// Event handler
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// User functions
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pickup", meta = (DisplayName = "Pickup"))
	void Pickup(class APS_Character* OwningCharacter);

private:
	// Component variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCapsuleComponent> CapsuleCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> Mesh;

	// Weapon variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	EHand Hand;

public:
	// Getter functions
	FORCEINLINE UCapsuleComponent* GetCapsuleCollision() const { return CapsuleCollision; }
	FORCEINLINE UStaticMeshComponent* GetMesh() const { return Mesh; }
	FORCEINLINE EHand GetHand() const { return Hand; }
};
