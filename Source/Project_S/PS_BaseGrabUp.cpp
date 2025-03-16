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
	//CapsuleCollision->SetupAttachment(RootComponent);
	CapsuleCollision->InitCapsuleSize(55.0f, 55.0f);
	CapsuleCollision->SetGenerateOverlapEvents(true);
	CapsuleCollision->bHiddenInGame = false;
	CapsuleCollision->SetCollisionProfileName(TEXT("Grabable"));
	CapsuleCollision->SetSimulatePhysics(true);
	CapsuleCollision->SetEnableGravity(true);
	*/

	// Setup Mesh Component
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	//Mesh->SetupAttachment(CapsuleCollision);
	Mesh->SetupAttachment(RootComponent);
	//Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	Mesh->SetCollisionProfileName(TEXT("Grabable"));
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(true);

	bIsActive = false;
	Owner = nullptr;
	PrimitiveRotation = FRotator::ZeroRotator;

	// Replication
	bReplicates = true;
}

// Called when the game starts or when spawned
void APS_BaseGrabUp::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APS_BaseGrabUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsActive && Owner) {
		// 캐릭터 몸통 앞에 위치
		//SetActorLocation(GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 200.0f);

		// 캐릭터 시선 방향에 위치
		FVector ViewVector = FVector(FMath::Cos(FMath::DegreesToRadians(Owner->GetControlRotation().Yaw)), FMath::Sin(FMath::DegreesToRadians(Owner->GetControlRotation().Yaw)), FMath::Tan(FMath::DegreesToRadians(Owner->GetControlRotation().Pitch)));
		//SetActorLocation(GetOwner()->GetActorLocation() + ViewVector * 500.0f + FVector(0.0f, 0.0f, 25.0f));

		FRotator NewRotation = FRotator(0.0f, Owner->GetControlRotation().Yaw, 0.0f) + PrimitiveRotation;
		//SetActorRotation(NewRotation);

		SetActorLocationandRotation(GetOwner()->GetActorLocation() + ViewVector * 250.0f + FVector(0.0f, 0.0f, 25.0f), NewRotation);
	}
}

// IPS_Grabable Interface functions
void APS_BaseGrabUp::Grab_Implementation(class APS_Character* CharacterInstigator)
{
	UE_LOG(Project_S, Log, TEXT("Grab\n"));
	bIsActive = true;
	//CapsuleCollision->SetSimulatePhysics(false);
	//CapsuleCollision->SetEnableGravity(false);
	Mesh->SetSimulatePhysics(false);
	Mesh->SetEnableGravity(false);
	SetOwner(CharacterInstigator);
	Owner = CharacterInstigator;
	PrimitiveRotation = GetActorRotation() - FRotator(0.0f, Owner->GetControlRotation().Yaw, 0.0f);
}

void APS_BaseGrabUp::UnGrab_Implementation(class APS_Character* CharacterInstigator)
{
	UE_LOG(Project_S, Log, TEXT("UnGrab\n"));
	bIsActive = false;
	//CapsuleCollision->SetSimulatePhysics(true);
	//CapsuleCollision->SetEnableGravity(true);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(true);
	SetOwner(nullptr);
	Owner = nullptr;
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

void APS_BaseGrabUp::SetActorLocationandRotation(FVector NewLocation, FRotator NewRotator)
{
	SetActorLocationandRotation_Server(NewLocation, NewRotator);
}

void APS_BaseGrabUp::SetActorLocationandRotation_Server_Implementation(FVector NewLocation, FRotator NewRotator)
{
	SetActorLocationandRotation_Client(NewLocation, NewRotator);
}

void APS_BaseGrabUp::SetActorLocationandRotation_Client_Implementation(FVector NewLocation, FRotator NewRotator)
{
	SetActorLocation(NewLocation);
	SetActorRotation(NewRotator);
}

