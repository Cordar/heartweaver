// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KrakenPushComponent.generated.h"


class AKrakenPushableActor;
class AKrakenCharacter;

UCLASS(BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HEARTWEAVER_API UKrakenPushComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UKrakenPushComponent();

	UFUNCTION(BlueprintCallable, Category = "Pushable")
	void BeginPush(AKrakenPushableActor* PushableObject);

	UFUNCTION(BlueprintCallable, Category = "Pushable")
	void EndPush();
	bool MakeCapsuleTrace(float Radius, float HalfHeight) const;

	UFUNCTION(BlueprintCallable, Category = "Pushable")
	bool IsHoldingObject() const { return CurrentPushableObject != nullptr; }

	UPROPERTY(BlueprintReadOnly, Category = "Pushable")
	float ForwardMove = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Pushable")
	float RightMove = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Pushable")
	bool IsMovingAnObject = false;

	UPROPERTY(BlueprintReadOnly, Category = "Pushable")
	FRotator PushRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Pushable")
	FVector PushVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pushable")
	float PushSpeed = 100.0f;

protected:
	virtual void BeginPlay() override;
	void MoveCurrentPushableObject(float DeltaTime);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	bool IsCharacterCollidingBackwards() const;
	void StopMovement();
	bool MakeLineTraceToSide(const bool CheckRight = false) const;

	UPROPERTY(EditAnywhere, Category = "Pushable")
	TObjectPtr<AKrakenPushableActor> CurrentPushableObject;

	UPROPERTY(VisibleAnywhere, Category = "Pushable")
	TObjectPtr<AKrakenCharacter> Character;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pushable")
	float PushRange = 120.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Pushable")
	float CurrentActorZRotation = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Pushable")
	bool CanMoveRight = true;

	UPROPERTY(VisibleAnywhere, Category = "Pushable")
	bool CanMoveLeft = true;

	UPROPERTY(VisibleAnywhere, Category = "Pushable")
	bool CanMoveUp = true;
	
	UPROPERTY(VisibleAnywhere, Category = "Pushable")
	bool CanMoveDown = true;

private:
	FVector GetDeltaLocation(float DeltaSecond, float InForwardMove, float InRightMove);
};
