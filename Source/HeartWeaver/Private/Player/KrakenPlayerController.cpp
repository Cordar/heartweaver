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

AKrakenPlayerController::AKrakenPlayerController()
{
	bReplicates = true;
}

void AKrakenPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(HeroContext);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
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

	ControlledCharacter = CastChecked<AKrakenCharacter>(GetPawn());
}

UKrakenAbilitySystemComponent* AKrakenPlayerController::GetKrakenAbilitySystemComponent()
{
	if (KrakenAbilitySystemComponent == nullptr && ControlledCharacter)
	{
		KrakenAbilitySystemComponent = Cast<UKrakenAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ControlledCharacter));
	}
	return KrakenAbilitySystemComponent;
}

void AKrakenPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, FString::Printf(TEXT("AbilityInputTagPressed: %s"), *InputTag.ToString()));
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
	check(InputComponent)

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

		KrakenInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &AKrakenPlayerController::Move, /*bLogIfNotFound=*/ false);
		KrakenInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Jump, ETriggerEvent::Started, this, &AKrakenPlayerController::Jump, /*bLogIfNotFound=*/ false);
		KrakenInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Jump, ETriggerEvent::Completed, this, &AKrakenPlayerController::StopJumping, /*bLogIfNotFound=*/
		                                       false);
		KrakenInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Crouch, ETriggerEvent::Triggered, this, &AKrakenPlayerController::ToggleCrouch,
		                                       /*bLogIfNotFound=*/ false);
		KrakenInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Climb, ETriggerEvent::Started, this, &AKrakenPlayerController::OnClimbActionStarted,
		                                       /*bLogIfNotFound=*/ false);

		KrakenInputComponent->BindAbilityActions(InputConfig, this, &AKrakenPlayerController::AbilityInputTagPressed, &AKrakenPlayerController::AbilityInputTagReleased,
		                                         &AKrakenPlayerController::AbilityInputTagHeld);
	}
}
