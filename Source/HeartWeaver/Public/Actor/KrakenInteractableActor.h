// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/KrakenActivableObject.h"
#include "KrakenInteractableActor.generated.h"

class USphereComponent;

UCLASS()
class HEARTWEAVER_API AKrakenInteractableActor : public AActor, public IKrakenActivableObject
{
	GENERATED_BODY()
	
public:	
	AKrakenInteractableActor();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
};
