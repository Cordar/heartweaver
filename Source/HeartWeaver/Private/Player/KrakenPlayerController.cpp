// Copyright Kraken's Den Studios. All rights reserved.


#include "Player/KrakenPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/KrakenAbilitySystemComponent.h"
#include "AbilitySystem/KrakenGameplayTags.h"
#include "Characters/KrakenCharacter.h"
#include "Input/KrakenInputComponent.h"
#include "Player/KrakenLocalPlayer.h"
#include "Characters/KrakenCharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AKrakenPlayerController::AKrakenPlayerController()
{
	bReplicates = true;
}

void AKrakenPlayerController::UpdateRotation(const float DeltaTime)
{
	FVector GravityDirection = FVector::DownVector;
	if (const ACharacter* PlayerCharacter = Cast<ACharacter>(GetPawn()))
	{
		if (UCharacterMovementComponent* MoveComp = PlayerCharacter->GetCharacterMovement())
		{
			GravityDirection = MoveComp->GetGravityDirection();
		}
	}

	// Get the current control rotation in world space
	FRotator ViewRotation = GetControlRotation();

	// Add any rotation from the gravity changes, if any happened.
	// Delete this code block if you don't want the camera to automatically compensate for gravity rotation.
	if (!LastFrameGravity.Equals(FVector::ZeroVector))
	{
		const FQuat DeltaGravityRotation = FQuat::FindBetweenNormals(LastFrameGravity, GravityDirection);
		const FQuat WarpedCameraRotation = DeltaGravityRotation * FQuat(ViewRotation);

		ViewRotation = WarpedCameraRotation.Rotator();
	}
	LastFrameGravity = GravityDirection;

	// Convert the view rotation from world space to gravity relative space.
	// Now we can work with the rotation as if no custom gravity was affecting it.
	ViewRotation = GetGravityRelativeRotation(ViewRotation, GravityDirection);

	// Calculate Delta to be applied on ViewRotation
	FRotator DeltaRot(RotationInput);

	if (PlayerCameraManager)
	{
		ACharacter* PlayerCharacter = Cast<ACharacter>(GetPawn());

		PlayerCameraManager->ProcessViewRotation(DeltaTime, ViewRotation, DeltaRot);

		// Zero the roll of the camera as we always want it horizontal in relation to the gravity.
		ViewRotation.Roll = 0;

		// Convert the rotation back to world space, and set it as the current control rotation.
		SetControlRotation(GetGravityWorldRotation(ViewRotation, GravityDirection));
	}

	if (APawn* const P = GetPawnOrSpectator())
	{
		P->FaceRotation(ViewRotation, DeltaTime);
	}
}

FRotator AKrakenPlayerController::GetGravityRelativeRotation(const FRotator& Rotation, const FVector& GravityDirection)
{
	if (!GravityDirection.Equals(FVector::DownVector))
	{
		const FQuat GravityRotation = FQuat::FindBetweenNormals(GravityDirection, FVector::DownVector);
		return (GravityRotation * Rotation.Quaternion()).Rotator();
	}

	return Rotation;
}

FRotator AKrakenPlayerController::GetGravityWorldRotation(const FRotator& Rotation, const FVector& GravityDirection)
{
	if (!GravityDirection.Equals(FVector::DownVector))
	{
		const FQuat GravityRotation = FQuat::FindBetweenNormals(FVector::DownVector, GravityDirection);
		return (GravityRotation * Rotation.Quaternion()).Rotator();
	}

	return Rotation;
}

void AKrakenPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(HeroContext);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(HeroContext, 0);
	}


	bShowMouseCursor = false;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AKrakenPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (AKrakenCharacter* KrakenCharacter = Cast<AKrakenCharacter>(InPawn))
	{
		ControlledCharacter = KrakenCharacter;
	}
	else
	{
		ControlledCharacter = nullptr;
	}
}

UKrakenAbilitySystemComponent* AKrakenPlayerController::GetKrakenAbilitySystemComponent()
{
	if (KrakenAbilitySystemComponent == nullptr && ControlledCharacter)
	{
		KrakenAbilitySystemComponent = Cast<UKrakenAbilitySystemComponent>(
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ControlledCharacter));
	}
	return KrakenAbilitySystemComponent;
}

void AKrakenPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (GetKrakenAbilitySystemComponent() == nullptr) return;

	GetKrakenAbilitySystemComponent()->AbilityInputTagPressed(InputTag);
}

void AKrakenPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetKrakenAbilitySystemComponent() == nullptr) return;

	GetKrakenAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
}

void AKrakenPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetKrakenAbilitySystemComponent() == nullptr) return;

	GetKrakenAbilitySystemComponent()->AbilityInputTagHeld(InputTag);
}

void AKrakenPlayerController::Move(const FInputActionValue& Value)
{
	if (ControlledCharacter)
	{
		ControlledCharacter->Move(Value);
	}
}

void AKrakenPlayerController::AimMarkWithJoystick(const FInputActionValue& Value)
{
	if (ControlledCharacter)
	{
		// bShowMouseCursor = false;
		//
		// FVector2D CenterPosition;
		// UGameplayStatics::ProjectWorldToScreen(this, ControlledCharacter->GetActorLocation(), CenterPosition);

		FVector2D Input = Value.Get<FVector2D>();
		Input.Normalize();

		AimMarkWithJoystickEvent.Broadcast(Input);
		// Input *= 60.0f;
		// SetMouseLocation(CenterPosition.X + Input.X, CenterPosition.Y + Input.Y);
		
	}
}

void AKrakenPlayerController::MoveStarted()
{
	if (ControlledCharacter)
	{
		ControlledCharacter->MoveStarted();
	}
}

void AKrakenPlayerController::MoveEnded()
{
	if (ControlledCharacter)
	{
		ControlledCharacter->MoveEnded();
	}
}

void AKrakenPlayerController::Jump()
{
	if (ControlledCharacter)
	{
		ControlledCharacter->Jump();
	}
}

void AKrakenPlayerController::StopJumping()
{
	if (ControlledCharacter)
	{
		ControlledCharacter->StopJumping();
	}
}

void AKrakenPlayerController::ToggleCrouch()
{
	if (ControlledCharacter)
	{
		ControlledCharacter->ToggleCrouch();
	}
}

void AKrakenPlayerController::OnClimbActionStarted(const FInputActionValue& Value)
{
	if (!ControlledCharacter && !ControlledCharacter->GetKrakenCharacterMovementComponent()) return;

	UKrakenCharacterMovementComponent* KrakenCharMoveComp = ControlledCharacter->GetKrakenCharacterMovementComponent();

	if (!KrakenCharMoveComp->IsClimbing())
	{
		KrakenCharMoveComp->ToggleClimbing(true);
	}
	else
	{
		KrakenCharMoveComp->ToggleClimbing(false);
	}
}

void AKrakenPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	check(InputComponent);

	const UKrakenLocalPlayer* Lp = Cast<UKrakenLocalPlayer>(GetLocalPlayer());
	check(Lp);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = Lp->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		Subsystem->ClearAllMappings();
	}

	if (InputConfig)
	{
		const FKrakenGameplayTags& GameplayTags = FKrakenGameplayTags::Get();

		UKrakenInputComponent* KrakenInputComponent = CastChecked<UKrakenInputComponent>(InputComponent);

		KrakenInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this,
		                                       &AKrakenPlayerController::Move, /*bLogIfNotFound=*/ false);
		KrakenInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_AimMarkWithJoystick,
		                                       ETriggerEvent::Triggered, this,
		                                       &AKrakenPlayerController::AimMarkWithJoystick, /*bLogIfNotFound=*/
		                                       false);
		KrakenInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Started, this,
		                                       &AKrakenPlayerController::MoveStarted, /*bLogIfNotFound=*/ false);
		KrakenInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Completed, this,
		                                       &AKrakenPlayerController::MoveEnded, /*bLogIfNotFound=*/ false);
		KrakenInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Crouch, ETriggerEvent::Triggered,
		                                       this, &AKrakenPlayerController::ToggleCrouch,
		                                       /*bLogIfNotFound=*/ false);
		KrakenInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Climb, ETriggerEvent::Started, this,
		                                       &AKrakenPlayerController::OnClimbActionStarted,
		                                       /*bLogIfNotFound=*/ false);

		KrakenInputComponent->BindAbilityActions(InputConfig, this, &AKrakenPlayerController::AbilityInputTagPressed,
		                                         &AKrakenPlayerController::AbilityInputTagReleased,
		                                         &AKrakenPlayerController::AbilityInputTagHeld);
	}
}
