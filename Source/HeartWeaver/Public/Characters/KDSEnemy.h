// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/KDSCharacterBase.h"
#include "KDSEnemy.generated.h"

/**
 * 
 */
UCLASS()
class HEARTWEAVER_API AKDSEnemy : public AKDSCharacterBase
{
	GENERATED_BODY()
public:
	AKDSEnemy(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	
};
