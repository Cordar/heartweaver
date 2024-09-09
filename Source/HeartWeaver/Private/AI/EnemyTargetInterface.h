// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyTargetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UEnemyTargetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HEARTWEAVER_API IEnemyTargetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy Target Interface")
	FVector GetHighestPoint();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy Target Interface")
	FVector GetLowestPoint();
};
