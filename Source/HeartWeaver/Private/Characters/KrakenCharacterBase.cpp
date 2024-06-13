// Copyright Kraken's Den Studios. All rights reserved.


#include "Characters/KrakenCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/KrakenAbilitySystemComponent.h"
#include "Characters/KrakenCharacterMovementComponent.h"


AKrakenCharacterBase::AKrakenCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UKrakenCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	KrakenCharacterMovementComponent = CastChecked<UKrakenCharacterMovementComponent>(GetCharacterMovement());
	KrakenCharacterMovementComponent->GravityScale = 1.0f;
	KrakenCharacterMovementComponent->MaxAcceleration = 2400.0f;
	KrakenCharacterMovementComponent->BrakingFrictionFactor = 1.0f;
	KrakenCharacterMovementComponent->BrakingFriction = 6.0f;
	KrakenCharacterMovementComponent->GroundFriction = 8.0f;
	KrakenCharacterMovementComponent->BrakingDecelerationWalking = 1400.0f;
	KrakenCharacterMovementComponent->bUseControllerDesiredRotation = false;
	KrakenCharacterMovementComponent->bOrientRotationToMovement = true;
	KrakenCharacterMovementComponent->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
	KrakenCharacterMovementComponent->bConstrainToPlane = true;
	KrakenCharacterMovementComponent->bSnapToPlaneAtStart = true;
	KrakenCharacterMovementComponent->bAllowPhysicsRotationDuringAnimRootMotion = false;
	KrakenCharacterMovementComponent->GetNavAgentPropertiesRef().bCanCrouch = true; // Maybe remove it if we don't want the camera to change
	KrakenCharacterMovementComponent->bCanWalkOffLedgesWhenCrouching = true;
	KrakenCharacterMovementComponent->SetCrouchedHalfHeight(65.0f);
}

UAbilitySystemComponent* AKrakenCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AKrakenCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AKrakenCharacterBase::InitAbilityActorInfo()
{
}

void AKrakenCharacterBase::InitializePrimaryAttributes() const
{
	check(IsValid(GetAbilitySystemComponent()))
	check(DefaultPrimaryAttributes)
	
	const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(DefaultPrimaryAttributes, 1, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void AKrakenCharacterBase::AddCharacterAbilities()
{
	if (bAbilitiesInitialized) return;
	UKrakenAbilitySystemComponent* KrakenAbilitySystemComponent = Cast<UKrakenAbilitySystemComponent>(GetAbilitySystemComponent());
	if (!HasAuthority()) return;

	KrakenAbilitySystemComponent->AddCharacterAbilities(StartupAbilities);
	bAbilitiesInitialized = true;
}


