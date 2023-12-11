// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "KrakenCharacterBase.h"
#include "KrakenEnemy.generated.h"

/**
 * 
 */
UCLASS()
class HEARTWEAVER_API AKrakenEnemy : public AKrakenCharacterBase
{
	GENERATED_BODY()
public:
	AKrakenEnemy(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	
};
