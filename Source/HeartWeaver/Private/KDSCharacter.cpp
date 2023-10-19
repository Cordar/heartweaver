// Copyright Kraken's Den Studios. All rights reserved.


#include "KDSCharacter.h"

#include "AbilitySystem/KDSGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "Input/KDSInputComponent.h"
#include "Player/KDSLocalPlayer.h"
#include "Player/KDSPlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

AKDSCharacter::AKDSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AKDSCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MoveVector = Value.Get<FVector2D>();

	const FRotator Rotation = GetController()->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MoveVector.Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MoveVector.X);
}

void AKDSCharacter::ToggleCrouch(const FInputActionValue& Value)
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

void AKDSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const AKDSPlayerController* PC = GetController<AKDSPlayerController>();
	check(PC);

	const UKDSLocalPlayer* LP = Cast<UKDSLocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);
	
	Subsystem->ClearAllMappings();

	if (InputConfig) {
		const FKDSGameplayTags& GameplayTags = FKDSGameplayTags::Get();

		UKDSInputComponent* KDSIC = CastChecked<UKDSInputComponent>(PlayerInputComponent);
		KDSIC->AddInputMappings(InputConfig, Subsystem);

		KDSIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &AKDSCharacter::Move, /*bLogIfNotFound=*/ false);
		KDSIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Jump, ETriggerEvent::Started, this, &AKDSCharacter::Jump, /*bLogIfNotFound=*/ false);
		KDSIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Jump, ETriggerEvent::Completed, this, &AKDSCharacter::StopJumping, /*bLogIfNotFound=*/ false);
		KDSIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Crouch, ETriggerEvent::Triggered, this, &AKDSCharacter::ToggleCrouch, /*bLogIfNotFound=*/ false);
	}
}

