// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "KrakenAssetManagerStartupJob.h"

#include "KrakenAssetManager.generated.h"

/**
 * UKDSAssetManager
 *
 * Game implementation of the asset manager that overrides functionality and stores game-specific types.
 * This class is used by setting 'AssetManagerClassName' in DefaultEngine.ini.
 */
UCLASS(Config = Game)
class HEARTWEAVER_API UKrakenAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:

	UKrakenAssetManager();

	// Returns the AssetManager singleton object.
	static UKrakenAssetManager& Get();

protected:

	//~UAssetManager interface
	virtual void StartInitialLoading() override;

private:

	// Flushes the StartupJobs array. Processes all startup work.
	void DoAllStartupJobs();

	// Sets up the ability system
	void InitializeAbilitySystem();

	// Called periodically during loads, could be used to feed the status to a loading screen
	void UpdateInitialGameContentLoadPercent(float GameContentPercent);

	// The list of tasks to execute on startup. Used to track startup progress.
	TArray<FKrakenAssetManagerStartupJob> StartupJobs;
	
};
