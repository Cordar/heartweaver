// Copyright 2022 bstt, Inc. All Rights Reserved.

#include "ANA_CacheHelper.h"
#include "Interfaces/IPluginManager.h"
#include "JsonObjectConverter.h"
#include "Misc/FileHelper.h"
#include "../Debug/Log.h"

FString ANA_CacheHelper::GetCachePath()
{
	static FString cachePath = FPaths::ConvertRelativePathToFull(IPluginManager::Get().FindPlugin("AutoNodeArranger")->GetBaseDir() + "/cache.json");
	return cachePath;
}

FANA_CacheData ANA_CacheHelper::GetCacheData()
{
	static FString cachePath = GetCachePath();
	FString cacheString;
	FANA_CacheData cacheData;

	if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*cachePath))
	{
		FFileHelper::LoadFileToString(cacheString, *cachePath);

		if (FJsonObjectConverter::JsonObjectStringToUStruct(cacheString, &cacheData, 0, 0))
			return cacheData; // parse succeed
		else
			UE_LOG(LogAutoNodeArranger, Warning, TEXT("Cannot load cached data"));
	}
	else
		UE_LOG(LogAutoNodeArranger, Log, TEXT("Cache data not found here: %s"), *cachePath);

	SetCacheData(cacheData);

	return cacheData;
}

void ANA_CacheHelper::SetCacheData(const FANA_CacheData& cacheData)
{
	static FString cachePath = GetCachePath();
	FString cacheString;

	if (FJsonObjectConverter::UStructToJsonObjectString(cacheData, cacheString))
		FFileHelper::SaveStringToFile(cacheString, *cachePath);
	else
		UE_LOG(LogAutoNodeArranger, Warning, TEXT("Cannot write cache data here: %s"), *cachePath);
}