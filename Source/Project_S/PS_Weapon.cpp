// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_Weapon.h"

// Sets default values
APS_Weapon::APS_Weapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
	RootComponent = Weapon;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("/Game/Characters/Adventurers/Assets/sword_1handed.sword_1handed"));
	if (SkeletalMeshAsset.Succeeded())
	{
		Weapon->SetSkeletalMesh(SkeletalMeshAsset.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PS_Weapon SkeletalMesh missing"));
	}

	Weapon->SetCollisionProfileName(TEXT("NoCollision"));
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

