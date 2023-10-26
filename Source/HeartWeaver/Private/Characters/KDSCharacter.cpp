// Copyright Kraken's Den Studios. All rights reserved.


#include "Characters/KDSCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

AKDSCharacter::AKDSCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));  // Rotate mesh to be X forward since it is exported as Y forward.

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());


	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
}

void AKDSCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AKDSCharacter::ToggleCrouch()
{
	if (!bIsCrouched) {
		Crouch();
	}
	else {
		UnCrouch();
	}
}

void AKDSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

