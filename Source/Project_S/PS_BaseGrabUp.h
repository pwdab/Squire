// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PS_Grabable.h"
#include "PS_Character.h"
#include "PS_BaseGrabUp.generated.h"

UCLASS()
class PROJECT_S_API APS_BaseGrabUp : public AActor, public IPS_Grabable
{
	GENERATED_BODY()
	
public:	
	APS_BaseGrabUp();

	// Override functions
	virtual void Tick(float DeltaTime) override;

	// IPS_Grabable Interface functions
	virtual void Grab_Implementation(class APS_Character* CharacterInstigator) override;
	virtual void UnGrab_Implementation(class APS_Character* CharacterInstigator) override;
	virtual void Throw_Implementation(class APS_Character* CharacterInstigator) override;
	virtual bool CanGrab_Implementation(APS_Character* CharacterInstigator) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetActorLocationandRotation(FVector NewLocation, FRotator NewRotator);

	UFUNCTION(Server, Reliable)
	void SetActorLocationandRotation_Server(FVector NewLocation, FRotator NewRotator);

	UFUNCTION(NetMulticast, Reliable)
	void SetActorLocationandRotation_Client(FVector NewLocation, FRotator NewRotator);

private:
	/*
	// Component variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCapsuleComponent> CapsuleCollision;
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab", meta = (AllowPrivateAccess = "true"))
	bool bIsActive;

	ACharacter* Owner;

	FRotator PrimitiveRotation;

public:
	// Getter functions
	//FORCEINLINE UCapsuleComponent* GetCapsuleCollision() const { return CapsuleCollision; }
	FORCEINLINE UStaticMeshComponent* GetMesh() const { return Mesh; }
};
