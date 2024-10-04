// Fill out your copyright notice in the Description page of Project Settings.

#include "PS_Weapon.h"

// Sets default values
APS_Weapon::APS_Weapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WEAPON"));
	RootComponent = Mesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("/Game/Characters/Adventurers/Assets/sword_1handed.sword_1handed"));
	if (StaticMesh.Succeeded())
	{
		Mesh->SetStaticMesh(StaticMesh.Object);
	}
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Hand = EHand::Right;
}

// Called when the game starts or when spawned
void APS_Weapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APS_Weapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

