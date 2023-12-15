// Copyright Kraken's Den Studios. All rights reserved.


#include "Player/KrakenPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/KrakenGameplayTags.h"
#include "Characters/KrakenCharacter.h"
#include "Input/KrakenInputComponent.h"
#include "Player/KrakenLocalPlayer.h"
#include "DebugHelper.h"
#include "Characters/KrakenCharacterMovementComponent.h"

AKrakenPlayerController::AKrakenPlayerController()
{
	bReplicates = true;
}

void AKrakenPlayerController::BeginPlay()
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

void AKrakenPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledCharacter = CastChecked<AKrakenCharacter>(GetPawn());
}

void AKrakenPlayerController::Move(const FInputActionValue& Value)
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
	if(!ControlledCharacter && !ControlledCharacter->GetKrakenCharacterMovementComponent()) return;

	if(!ControlledCharacter && !ControlledCharacter->GetKrakenCharacterMovementComponent()->IsClimbing())
	{
		ControlledCharacter->GetKrakenCharacterMovementComponent()->ToggleClimbing(true);
	}
	else
	{
		ControlledCharacter->GetKrakenCharacterMovementComponent()->ToggleClimbing(false);

	}
	
}

void AKrakenPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	check(InputComponent)

	const UKrakenLocalPlayer* Lp = Cast<UKrakenLocalPlayer>(GetLocalPlayer());
	check(Lp);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = Lp->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);
	
	Subsystem->ClearAllMappings();

	if (InputConfig) {
		const FKrakenGameplayTags& GameplayTags = FKrakenGameplayTags::Get();

		UKrakenInputComponent* Kdsic = CastChecked<UKrakenInputComponent>(InputComponent);

		Kdsic->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &AKrakenPlayerController::Move, /*bLogIfNotFound=*/ false);
		
		Kdsic->BindNativeAction(InputConfig, GameplayTags.InputTag_Jump, ETriggerEvent::Started, this, &AKrakenPlayerController::Jump, /*bLogIfNotFound=*/ false);
		Kdsic->BindNativeAction(InputConfig, GameplayTags.InputTag_Jump, ETriggerEvent::Completed, this, &AKrakenPlayerController::StopJumping, /*bLogIfNotFound=*/ false);
		
		Kdsic->BindNativeAction(InputConfig, GameplayTags.InputTag_Crouch, ETriggerEvent::Triggered, this, &AKrakenPlayerController::ToggleCrouch, /*bLogIfNotFound=*/ false);

		Kdsic->BindNativeAction(InputConfig, GameplayTags.InputTag_Climb, ETriggerEvent::Started, this, &AKrakenPlayerController::OnClimbActionStarted, /*bLogIfNotFound=*/ false);
	}
}
