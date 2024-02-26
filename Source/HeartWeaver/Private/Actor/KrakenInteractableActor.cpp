// Copyright Kraken's Den Studios. All rights reserved.


#include "Actor/KrakenInteractableActor.h"

#include "Characters/KrakenCharacter.h"
#include "Components/WidgetComponent.h"

AKrakenInteractableActor::AKrakenInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);
}

void AKrakenInteractableActor::BeginPlay()
{
	Super::BeginPlay();	
}

void AKrakenInteractableActor::WasSetAsTarget_Implementation()
{
}

void AKrakenInteractableActor::SetAsTarget(AKrakenCharacter* TargetCharacter)
{
	if (TargetCharacter == nullptr) return;

	TargetCharacter->SetInteractableActor(this);

	WasSetAsTarget();
}

void AKrakenInteractableActor::WasUnsetAsTarget_Implementation()
{
}

void AKrakenInteractableActor::UnsetAsTarget(AKrakenCharacter* TargetCharacter)
{
	if (TargetCharacter == nullptr) return;
	TargetCharacter->SetInteractableActor(nullptr);

	WasUnsetAsTarget();
}


