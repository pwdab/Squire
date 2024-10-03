// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_BasePickup.h"
#include "PS_Character.h"
#include "PS_Weapon.h"
#include "Components/CapsuleComponent.h"

// Sets default values
APS_BasePickup::APS_BasePickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>("Collision");
	RootComponent = CapsuleCollision;
	CapsuleCollision->SetGenerateOverlapEvents(true);
	CapsuleCollision->bHiddenInGame = false;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(CapsuleCollision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetRelativeRotation(FRotator(0.0f, 0.0f, -10.0f));

	bReplicates = true;
}

// Called when the game starts or when spawned
void APS_BasePickup::BeginPlay()
{
	Super::BeginPlay();
	
	// Dynamic Multi-cast Delegate
	CapsuleCollision->OnComponentBeginOverlap.AddDynamic(this, &APS_BasePickup::OnBeginOverlap);

	// Weapon ¼³Á¤
	//WeaponItemClass = APS_Weapon::StaticClass();
}

// Called every frame
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
	if (WeaponItemClass != nullptr)
	{
		if (OwningCharacter->CanSetWeapon(Hand))
		{
			auto NewWeapon = GetWorld()->SpawnActor<APS_Weapon>(WeaponItemClass, FVector::ZeroVector, FRotator::ZeroRotator);
			OwningCharacter->SetWeapon(NewWeapon, Hand);
		}
	}
	
	//SetOwner(OwningCharacter);
	/*
	auto NewWeapon = GetWorld()->SpawnActor<APS_Weapon>(WeaponItemClass, FVector::ZeroVector, FRotator::ZeroRotator);
	SetWeapon(NewWeapon);
	OwningCharacter
	*/
}
