// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ANA_CacheHelper.generated.h"

USTRUCT()
struct FANA_CacheData
{
	GENERATED_BODY()

	UPROPERTY()
		int32 version = 0;

	UPROPERTY()
		FDateTime laterTime = 0;
};

class ANA_CacheHelper
{
	ANA_CacheHelper() = delete;

public:
	static FANA_CacheData GetCacheData();
	static void SetCacheData(const FANA_CacheData& cacheData);

private:
	static FString GetCachePath();
};
