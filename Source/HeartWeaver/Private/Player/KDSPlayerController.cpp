// Copyright Kraken's Den Studios. All rights reserved.


#include "Player/KDSPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/KDSGameplayTags.h"
#include "Characters/KDSCharacter.h"
#include "Input/KDSInputComponent.h"
#include "Player/KDSLocalPlayer.h"

AKDSPlayerController::AKDSPlayerController()
{
	bReplicates = true;
}

void AKDSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	check(HeroContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);
	Subsystem->AddMappingContext(HeroContext, 0);

	bShowMouseCursor = false;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AKDSPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledCharacter = CastChecked<AKDSCharacter>(GetPawn());
}

void AKDSPlayerController::Move(const FInputActionValue& Value)
{
	const FVector2D MoveVector = Value.Get<FVector2D>();

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (ControlledCharacter)
	{
		ControlledCharacter->AddMovementInput(ForwardDirection, MoveVector.Y);
		ControlledCharacter->AddMovementInput(RightDirection, MoveVector.X);
	}
}

void AKDSPlayerController::Jump()
{
	if (ControlledCharacter)
	{
		ControlledCharacter->Jump();
	}
}

void AKDSPlayerController::StopJumping()
{
	if (ControlledCharacter)
	{
		ControlledCharacter->StopJumping();
	}
}

void AKDSPlayerController::ToggleCrouch()
{
	if (ControlledCharacter)
	{
		ControlledCharacter->ToggleCrouch();
	}
}

void AKDSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	check(InputComponent)

	const UKDSLocalPlayer* Lp = Cast<UKDSLocalPlayer>(GetLocalPlayer());
	check(Lp);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = Lp->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);
	
	Subsystem->ClearAllMappings();

	if (InputConfig) {
		const FKDSGameplayTags& GameplayTags = FKDSGameplayTags::Get();

		UKDSInputComponent* Kdsic = CastChecked<UKDSInputComponent>(InputComponent);

		Kdsic->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &AKDSPlayerController::Move, /*bLogIfNotFound=*/ false);
		Kdsic->BindNativeAction(InputConfig, GameplayTags.InputTag_Jump, ETriggerEvent::Started, this, &AKDSPlayerController::Jump, /*bLogIfNotFound=*/ false);
		Kdsic->BindNativeAction(InputConfig, GameplayTags.InputTag_Jump, ETriggerEvent::Completed, this, &AKDSPlayerController::StopJumping, /*bLogIfNotFound=*/ false);
		Kdsic->BindNativeAction(InputConfig, GameplayTags.InputTag_Crouch, ETriggerEvent::Triggered, this, &AKDSPlayerController::ToggleCrouch, /*bLogIfNotFound=*/ false);
	}
}
