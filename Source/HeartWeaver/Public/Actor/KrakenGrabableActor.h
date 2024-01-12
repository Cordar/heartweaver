// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KrakenGrabableActor.generated.h"

class AKrakenCharacter;
class USphereComponent;

UENUM(BlueprintType)
enum class EGrabableType
{
	Pickup,
	Movable
};


UCLASS()
class HEARTWEAVER_API AKrakenGrabableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AKrakenGrabableActor();

	UFUNCTION(BlueprintCallable, Category = "Grabable")
	void Release(AKrakenCharacter* TargetCharacter);

	UFUNCTION(BlueprintCallable, Category = "Grabable")
	void Grab(AKrakenCharacter* TargetCharacter);
	
protected:
	virtual void BeginPlay() override;		

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grabable")
	EGrabableType GrabableType;

private:	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
};
