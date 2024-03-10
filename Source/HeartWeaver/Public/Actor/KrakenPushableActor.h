// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/KrakenPushableObject.h"
#include "KrakenPushableActor.generated.h"

UCLASS()
class HEARTWEAVER_API AKrakenPushableActor : public AActor, public IKrakenPushableObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKrakenPushableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
