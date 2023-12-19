// Copyright Kraken's Den Studios. All rights reserved.


#include "Characters/KrakenCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/KrakenAbilitySystemComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Characters/KrakenCharacterMovementComponent.h"
#include "Player/KrakenPlayerController.h"
#include "Player/KrakenPlayerState.h"
#include "UI/HUD/KrakenHUD.h"

AKrakenCharacter::AKrakenCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UKrakenCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));  // Rotate mesh to be X forward since it is exported as Y forward.

	UKrakenCharacterMovementComponent* KDSMoveComp = CastChecked<UKrakenCharacterMovementComponent>(GetCharacterMovement());
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
	KDSMoveComp->GetNavAgentPropertiesRef().bCanCrouch = true; // Maybe remove it if we don't want the camera to change
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

void AKrakenCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AKrakenCharacter::ToggleCrouch()
{
	const UKrakenCharacterMovementComponent* MoveComp = CastChecked<UKrakenCharacterMovementComponent>(GetCharacterMovement());

	if (bIsCrouched || MoveComp->bWantsToCrouch)
	{
		UnCrouch();
	}
	else if (MoveComp->IsMovingOnGround())
	{
		Crouch();
	}
}

bool AKrakenCharacter::CanJumpInternal_Implementation() const
{
	// same as ACharacter's implementation but without the crouch check
	return JumpIsAllowedInternal();
}

void AKrakenCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AKrakenCharacter::InitAbilityActorInfo()
{
	AKrakenPlayerState* KrakenPlayerState = GetPlayerState<AKrakenPlayerState>();
	check(KrakenPlayerState);
	KrakenPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(KrakenPlayerState, this);
	Cast<UKrakenAbilitySystemComponent>(KrakenPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = KrakenPlayerState->GetAbilitySystemComponent();
	AttributeSet = KrakenPlayerState->GetAttributeSet();

	if (AKrakenPlayerController* KrakenPlayerController = Cast<AKrakenPlayerController>(GetController()))
	{
		if (AKrakenHUD* KrakenHUD = Cast<AKrakenHUD>(KrakenPlayerController->GetHUD()))
		{
			KrakenHUD->InitOverlay(KrakenPlayerController, KrakenPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}

	InitializePrimaryAttributes();
}

void AKrakenCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init Ability Actor Info for the server
	InitAbilityActorInfo();
}

void AKrakenCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init Ability Actor Info for the client
	InitAbilityActorInfo();
}

