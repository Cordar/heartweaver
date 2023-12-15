// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KrakenCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
namespace ECustomMovementMode
{
	enum Type
	{
		Move_Climb UMETA(DisplayName = "Climb Mode")
	};
}

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

	void ToggleClimbing(bool bEnableClimb);
	bool IsClimbing() const;

protected:
	//~Climb BPVariables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	TArray<TEnumAsByte<EObjectTypeQuery> > ClimbableSurfaceTraceTypes;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	float ClimbCapsuleTraceRadius = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	float ClimbCapsuleTraceHalfHeight = 72.f;
	//~End of Climb BPVariables
	
private:
	//~Climb Traces
	TArray<FHitResult> DoCapsuleTraceMultiByObject(const FVector& Start,const FVector& End,bool bShowDebugShape = false,bool bDrawPersistentShapes = false);
	FHitResult DoLineTraceSingleByObject(const FVector& Start, const FVector& End, bool bShowDebugShape = false,bool bDrawPersistentShapes = false) const;
	//~End of ClimbTraces

	//~ClimbCore
	bool TraceClimbableSurfaces();

	FHitResult TraceFromEyeHeight(float TraceDistance,float TraceStartOffset = 0.f) const;

	bool CanStartClimbing();
	//~End of ClimbCore

	//~ClimbCoreVariables
	TArray<FHitResult> ClimbableSurfacesTraceResults;
	//~End of ClimbCoreVariables

};
