// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KrakenCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class HEARTWEAVER_API UKrakenCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	UKrakenCharacterMovementComponent();

	virtual bool CanAttemptJump() const override;

	//~UMovementComponent interface
	virtual FRotator GetDeltaRotation(float DeltaTime) const override;
	virtual float GetMaxSpeed() const override;
	//~End of UMovementComponent interface
	
};
