// Copyright Kraken's Den Studios. All rights reserved.


#include "Characters/KDSCharacter.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Characters/KDSCharacterMovementComponent.h"
#include "Player/KDSPlayerState.h"

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
	KDSMoveComp->bOrientRotationToMovement = true;
	KDSMoveComp->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
	KDSMoveComp->bConstrainToPlane = true;
	KDSMoveComp->bSnapToPlaneAtStart = true;
	KDSMoveComp->bAllowPhysicsRotationDuringAnimRootMotion = false;
	KDSMoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	KDSMoveComp->bCanWalkOffLedgesWhenCrouching = true;
	KDSMoveComp->SetCrouchedHalfHeight(65.0f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->SetupAttachment(GetRootComponent());

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);

	BaseEyeHeight = 80.0f;
	CrouchedEyeHeight = 50.0f;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

void AKDSCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AKDSCharacter::ToggleCrouch()
{
	const UKDSCharacterMovementComponent* MoveComp = CastChecked<UKDSCharacterMovementComponent>(GetCharacterMovement());

	if (bIsCrouched || MoveComp->bWantsToCrouch)
	{
		UnCrouch();
	}
	else if (MoveComp->IsMovingOnGround())
	{
		Crouch();
	}
}

bool AKDSCharacter::CanJumpInternal_Implementation() const
{
	// same as ACharacter's implementation but without the crouch check
	return JumpIsAllowedInternal();
}

void AKDSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AKDSCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init Ability Actor Info for the server
	AKDSPlayerState* KDSPlayerState = GetPlayerState<AKDSPlayerState>();
	check(KDSPlayerState);
	KDSPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(KDSPlayerState, this);
	AbilitySystemComponent = KDSPlayerState->GetAbilitySystemComponent();
	AttributeSet = KDSPlayerState->GetAttributeSet();
}

void AKDSCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

