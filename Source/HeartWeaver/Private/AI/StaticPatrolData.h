#pragma once

#include "StaticPatrolData.generated.h"

USTRUCT(BlueprintType)
struct FStaticPatrolData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float WaitTime = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AngleOffset = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float TransitionTime = 0.0f;
};
