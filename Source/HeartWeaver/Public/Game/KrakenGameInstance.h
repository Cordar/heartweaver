// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "KrakenGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class HEARTWEAVER_API UKrakenGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
};
