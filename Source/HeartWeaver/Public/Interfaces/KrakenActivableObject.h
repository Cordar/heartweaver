// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "KrakenActivableObject.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UKrakenActivableObject : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HEARTWEAVER_API	IKrakenActivableObject
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Activable")
	void Activate(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Activable")
	void Deactivate();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Activable")
	bool CanBeActivatedBetweenWalls();
};
