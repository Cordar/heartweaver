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
	//~End of UMovementComponent interface

	void ToggleClimbing(bool bEnableClimb);
	bool IsClimbing() const;
	FORCEINLINE FVector GetClimbableSurfaceNormal() const { return CurrentClimbableSurfaceNormal; }
	FVector GetUnrotatedClimbVelocity()	const;

protected:
	//~ClimbBPVariables	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	float ClimbCapsuleTraceRadius = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	float ClimbCapsuleTraceHalfHeight = 72.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	float MaxBreakClimbDeceleration = 400.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	float MaxClimbSpeed = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	float MaxClimbAcceleration = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	TObjectPtr<UAnimMontage> IdleToClimbMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	TObjectPtr<UAnimMontage> ClimbToTopMontage;
	//~End of ClimbBPVariables

	//~UCharacterMovementComponent Interface
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

	virtual float GetMaxSpeed() const override;

	virtual float GetMaxAcceleration() const override;

	virtual FVector ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity, const FVector& CurrentVelocity) const override;
	//~End of UCharacterMovementComponent Interface

private:
	//~Climb Traces
	TArray<FHitResult> DoCapsuleTraceMultiByObject(const FVector& Start,const FVector& End,bool bShowDebugShape = false,bool bDrawPersistentShapes = false);

	FHitResult DoLineTraceSingleByObject(const FVector& Start, const FVector& End, bool bShowDebugShape = false,bool bDrawPersistentShapes = false) const;
	//~End of ClimbTraces

	//~ClimbCore
	bool TraceClimbableSurfaces();
	
	FHitResult TraceFromEyeHeight(float TraceDistance,float TraceStartOffset = 0.f) const;
	
	bool CanStartClimbing();
	
	void StartClimbing();
	
	void StopClimbing();
	
	void PhysClimb(float DeltaTime, int32 Iterations);

	void ProcessClimbableSurfaceInfo();

	bool CheckShouldStopClimbing();

	bool CheckHasReachedFloor();

	FQuat GetClimbRotation(float DeltaTime);

	void SnapMovementToClimbableSurfaces(float DeltaTime);

	bool CheckHasReachedLedge();

	void PlayClimbMontage(UAnimMontage* MontageToPlay);

	UFUNCTION()
	void OnClimbMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	//~End of ClimbCore

	//~ClimbCoreVariables
	TArray<FHitResult> ClimbableSurfacesTraceResults;

	FVector CurrentClimbableSurfaceLocation;

	FVector CurrentClimbableSurfaceNormal;

	UPROPERTY()
	TObjectPtr<UAnimInstance> OwningCharacterAnimInstance;
	//~End of ClimbCoreVariables

};
