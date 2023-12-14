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
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~End of UMovementComponent interface

protected:
	//~Climb Variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	TArray<TEnumAsByte<EObjectTypeQuery> > ClimbableSurfaceTraceTypes;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	float ClimbCapsuleTraceRadius = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	float ClimbCapsuleTraceHalfHeight = 72.f;
	//~End of Climb Variables
	
private:
	//~Climb Traces
	TArray<FHitResult> DoCapsuleTraceMultiByObject(const FVector& Start, const FVector& End, bool bShowDebugShape = false);
	//~End of ClimbTraces

	//~Climb Core
	void TraceClimbableSurfaces();
};
