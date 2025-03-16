// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_BasePickup.h"
#include "PS_Character.h"
#include "PS_Weapon.h"
#include "Components/CapsuleComponent.h"

APS_BasePickup::APS_BasePickup()
{
	PrimaryActorTick.bCanEverTick = true;

	// Setup Capsule Component
	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>("Collision");
	RootComponent = CapsuleCollision;
	CapsuleCollision->SetGenerateOverlapEvents(true);
	CapsuleCollision->bHiddenInGame = false;
	
	// Setup Mesh Component
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(CapsuleCollision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetRelativeRotation(FRotator(0.0f, 0.0f, -10.0f));

	// Replication
	bReplicates = true;
}

void APS_BasePickup::BeginPlay()
{
	Super::BeginPlay();
	
	// Register Delegate
	CapsuleCollision->OnComponentBeginOverlap.AddDynamic(this, &APS_BasePickup::OnBeginOverlap);
}

void APS_BasePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APS_BasePickup::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const auto Character = Cast<APS_Character>(OtherActor))
	{
		Pickup(Character);
	}
}

void APS_BasePickup::Pickup_Implementation(class APS_Character* OwningCharacter)
{
	if (WeaponItemClass && OwningCharacter->CanSetWeapon(Hand))
	{
		auto NewWeapon = GetWorld()->SpawnActor<APS_Weapon>(WeaponItemClass, FVector::ZeroVector, FRotator::ZeroRotator);
		OwningCharacter->SetWeapon(NewWeapon, Hand);
	}
}
