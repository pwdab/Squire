// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_BaseGrabUp.h"
#include "PS_Character.h"
#include "Components/CapsuleComponent.h"

// Sets default values
APS_BaseGrabUp::APS_BaseGrabUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*
	// Setup Capsule Component
	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>("Collision");
	RootComponent = CapsuleCollision;
	CapsuleCollision->SetGenerateOverlapEvents(true);
	CapsuleCollision->bHiddenInGame = false;
	*/

	// Setup Mesh Component
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	//Mesh->SetupAttachment(CapsuleCollision);
	Mesh->SetupAttachment(RootComponent);
	//Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	bIsActive = false;
}

// Called when the game starts or when spawned
void APS_BaseGrabUp::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(Project_S, Log, TEXT("GrabUp Begin Play\n"));
}

// Called every frame
void APS_BaseGrabUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsActive) {
		SetActorLocation(GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 200.0f);
	}
}

// IPS_Grabable Interface functions
void APS_BaseGrabUp::Grab_Implementation(class APS_Character* CharacterInstigator)
{
	UE_LOG(Project_S, Log, TEXT("Grab\n"));
	bIsActive = true;
	SetOwner(CharacterInstigator);
}

void APS_BaseGrabUp::UnGrab_Implementation(class APS_Character* CharacterInstigator)
{
	UE_LOG(Project_S, Log, TEXT("UnGrab\n"));
	bIsActive = false;
	SetOwner(nullptr);
}

void APS_BaseGrabUp::Throw_Implementation(class APS_Character* CharacterInstigator)
{
	UE_LOG(Project_S, Log, TEXT("Throw\n"));
}

bool APS_BaseGrabUp::CanGrab_Implementation(APS_Character* CharacterInstigator) const
{
	UE_LOG(Project_S, Log, TEXT("CanGrab\n"));
	return true;
}

