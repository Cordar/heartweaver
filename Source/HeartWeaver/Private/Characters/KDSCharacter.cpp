// Copyright Kraken's Den Studios. All rights reserved.


#include "Characters/KDSCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Characters/KDSCharacterMovementComponent.h"

AKDSCharacter::AKDSCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UKDSCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));  // Rotate mesh to be X forward since it is exported as Y forward.

	UKDSCharacterMovementComponent* KDSMoveComp = CastChecked<UKDSCharacterMovementComponent>(GetCharacterMovement());
	KDSMoveComp->GravityScale = 1.0f;
	KDSMoveComp->MaxAcceleration = 2400.0f;
	KDSMoveComp->BrakingFrictionFactor = 1.0f;
	KDSMoveComp->BrakingFriction = 6.0f;
	KDSMoveComp->GroundFriction = 8.0f;
	KDSMoveComp->BrakingDecelerationWalking = 1400.0f;
	KDSMoveComp->bUseControllerDesiredRotation = false;
	KDSMoveComp->bOrientRotationToMovement = false;
	KDSMoveComp->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	KDSMoveComp->bAllowPhysicsRotationDuringAnimRootMotion = false;
	KDSMoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	KDSMoveComp->bCanWalkOffLedgesWhenCrouching = true;
	KDSMoveComp->SetCrouchedHalfHeight(65.0f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);

	BaseEyeHeight = 80.0f;
	CrouchedEyeHeight = 50.0f;
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

