// Copyright Kraken's Den Studios. All rights reserved.


#include "Characters/KrakenCharacter.h"

#include "AbilitySystemComponent.h"
#include "InputActionValue.h"
#include "AbilitySystem/KrakenAbilitySystemComponent.h"
#include "Camera/CameraActor.h"
#include "Characters/KrakenCharacterMovementComponent.h"
#include "Characters/KrakenPushComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/KrakenPlayerController.h"
#include "Player/KrakenPlayerState.h"
#include "UI/HUD/KrakenHUD.h"

AKrakenCharacter::AKrakenCharacter(const FObjectInitializer& ObjectInitializer)
	: AKrakenCharacterBase(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));  // Rotate mesh to be X forward since it is exported as Y forward.

	BaseEyeHeight = 80.0f;
	CrouchedEyeHeight = 50.0f;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	KrakenPushComponent = CreateDefaultSubobject<UKrakenPushComponent>(TEXT("Push Component"));
}

void AKrakenCharacter::SetEmotionState(const EEmotionState NewEmotionState)
{
	EmotionState = NewEmotionState;
}

void AKrakenCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(
		SaveSafeLocationTimerHandle, // handle to cancel timer at a later time
		this, // the owning object
		&AKrakenCharacter::SaveLastSafeLocation, // function to call on elapsed
		0.5f, // float delay until elapsed
		true); // looping?
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

void AKrakenCharacter::Move(const FInputActionValue& Value)
{
	if (!GetKrakenCharacterMovementComponent()) return;

	if (GetKrakenCharacterMovementComponent()->IsClimbing())
	{
		HandleClimbMovementInput(Value);
	}
	else if(KrakenPushComponent->IsHoldingObject())
	{
		HandlePushMovementInput(Value);
	}
	else
	{
		HandleGroundMovementInput(Value);
	}
}

void AKrakenCharacter::SetInteractableActor(AKrakenInteractableActor* InteractableActor)
{
	KrakenInteractableActor = InteractableActor;
}

bool AKrakenCharacter::IsLayingOnFloor() const
{
	return bIsLayingOnFloor;
}

void AKrakenCharacter::SetLayingOnFloor(bool bNewIsLayingOnFloor)
{
	bIsLayingOnFloor = bNewIsLayingOnFloor;
}

void AKrakenCharacter::HandleGroundMovementInput(const FInputActionValue& Value)
{
	if (Controller == nullptr) return;
	
	if (FollowCamera == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("FollowCamera is not set in KrakenCharacter. Cannot handle movement input."));
		return;
	}
	
	const FVector2D MoveVector = Value.Get<FVector2D>();

	const FVector GravityDirection = GetGravityDirection(); // Assuming GetGravityDirection returns the direction of gravity

	// Calculate the forward and right directions based on the gravity direction and the camera's orientation
	const FVector ForwardDirection = FVector::CrossProduct(GravityDirection, FollowCamera->GetActorRightVector()).GetSafeNormal();
	const FVector RightDirection = FVector::CrossProduct(GravityDirection, FollowCamera->GetActorUpVector()).GetSafeNormal();

	// Add movement input in the calculated directions
	AddMovementInput(ForwardDirection, MoveVector.Y);
	AddMovementInput(RightDirection, MoveVector.X);
}

void AKrakenCharacter::HandlePushMovementInput(const FInputActionValue& Value)
{
	if (Controller == nullptr) return;
	
	const FVector2D MoveVector = Value.Get<FVector2D>();

	KrakenPushComponent->ForwardMove = MoveVector.Y;
	KrakenPushComponent->RightMove = MoveVector.X;
}

void AKrakenCharacter::HandleClimbMovementInput(const FInputActionValue& Value)
{
	if (Controller == nullptr) return;
	
	const FVector2D MoveVector = Value.Get<FVector2D>();
	const FVector ForwardDirection = FVector::CrossProduct(
		-GetKrakenCharacterMovementComponent()->GetClimbableSurfaceNormal(),
		GetActorRightVector()
		);
	const FVector RightDirection = FVector::CrossProduct(
	GetKrakenCharacterMovementComponent()->GetClimbableSurfaceNormal(),
	GetActorUpVector()
		);

	AddMovementInput(ForwardDirection, MoveVector.Y);
	AddMovementInput(RightDirection, MoveVector.X);
}

void AKrakenCharacter::SaveLastSafeLocation()
{
	if (KrakenCharacterMovementComponent->MovementMode == MOVE_Walking)
	{
		LastSafeLocation = SafeLocation;
		SafeLocation = GetActorLocation();
	}
}

void AKrakenCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init Ability Actor Info for the server
	InitAbilityActorInfo();
	AddCharacterAbilities();
}

void AKrakenCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init Ability Actor Info for the client
	InitAbilityActorInfo();
}

