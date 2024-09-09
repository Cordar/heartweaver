#pragma once

#include "StaticPatrolData.generated.h"

USTRUCT(BlueprintType)
struct FStaticPatrolData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float WaitTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AngleOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float TransitionTime;
};
