// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "KrakenPushableObject.generated.h"

class AKrakenCharacter;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UKrakenPushableObject : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HEARTWEAVER_API IKrakenPushableObject
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pushable")
	void Hold(AKrakenCharacter* Character);
};
