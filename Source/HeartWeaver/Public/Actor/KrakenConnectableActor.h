// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/KrakenActivableObject.h"
#include "KrakenConnectableActor.generated.h"

UCLASS()
class HEARTWEAVER_API AKrakenConnectableActor : public AActor, public IKrakenActivableObject
{
	GENERATED_BODY()
	
public:
	AKrakenConnectableActor();

protected:
	virtual void BeginPlay() override;

};
