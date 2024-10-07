// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Project_S.h"
#include "GameFramework/Actor.h"
#include "PS_BasePickup.generated.h"

UENUM(BlueprintType)
enum class EHand : uint8
{
	Bare_Handed = 0 UMETA(DisplayName = "Bare_Handed"),
	Left_Handed = 1 UMETA(DisplayName = "Left_Handed"),
	Right_Handed = 2 UMETA(DisplayName = "Right_Handed"),
	Two_Handed = 3 UMETA(DisplayName = "Two_Handed")
};

UCLASS()
class PROJECT_S_API APS_BasePickup : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCapsuleComponent> CapsuleCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	EHand Hand;
	
public:	
	// Sets default values for this actor's properties
	APS_BasePickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pickup", meta = (DisplayName = "Pickup"))
	void Pickup(class APS_Character* OwningCharacter);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UPROPERTY(EditInstanceOnly, Category = Weapon)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components)
	TSubclassOf<class APS_Weapon> WeaponItemClass;

	// Getter functions
	FORCEINLINE UCapsuleComponent* GetCapsuleCollision() const { return CapsuleCollision; }
	FORCEINLINE UStaticMeshComponent* GetMesh() const { return Mesh; }
	FORCEINLINE EHand GetHand() const { return Hand; }
};
