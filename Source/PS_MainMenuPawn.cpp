// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_MainMenuPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"

// Sets default values
APS_MainMenuPawn::APS_MainMenuPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	RootComponent = Arrow;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);
	Camera->SetRelativeLocation(FVector(0.0f, 0.0f, 1000.0f));
	Camera->SetRelativeRotation(FRotator(-90.0f, 180.0f, 0.0f));

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SkeletalMesh->SetupAttachment(RootComponent);
	SkeletalMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	SkeletalMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("/Game/Characters/Adventurers/Knight/Knight.Knight"));
	if (SkeletalMeshAsset.Succeeded())
	{
		SkeletalMesh->SetSkeletalMesh(SkeletalMeshAsset.Object);
	}
}

// Called when the game starts or when spawned
void APS_MainMenuPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APS_MainMenuPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APS_MainMenuPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

