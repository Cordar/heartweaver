// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "KDSCharacterBase.generated.h"

UCLASS(Abstract)
class HEARTWEAVER_API AKDSCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AKDSCharacterBase();

protected:
	virtual void BeginPlay() override;

};
