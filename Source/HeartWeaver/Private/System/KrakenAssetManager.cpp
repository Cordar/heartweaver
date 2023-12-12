// Copyright Kraken's Den Studios. All rights reserved.


#include "System/KrakenAssetManager.h"

#include "AbilitySystem/KrakenGameplayTags.h"

//////////////////////////////////////////////////////////////////////

#define STARTUP_JOB_WEIGHTED(JobFunc, JobWeight) StartupJobs.Add(FKrakenAssetManagerStartupJob(#JobFunc, [this](const FKrakenAssetManagerStartupJob& StartupJob, TSharedPtr<FStreamableHandle>& LoadHandle){JobFunc;}, JobWeight))
#define STARTUP_JOB(JobFunc) STARTUP_JOB_WEIGHTED(JobFunc, 1.f)

//////////////////////////////////////////////////////////////////////

UKrakenAssetManager::UKrakenAssetManager()
{
}

UKrakenAssetManager& UKrakenAssetManager::Get()
{
	check(GEngine);

	if (UKrakenAssetManager* Singleton = Cast<UKrakenAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini.  It must be set to KDSAssetManager!"));

	// Fatal error above prevents this from being called.
	return *NewObject<UKrakenAssetManager>();
}

void UKrakenAssetManager::StartInitialLoading()
{
	SCOPED_BOOT_TIMING("UKrakenAssetManager::StartInitialLoading");

	// This does all of the scanning, need to do this now even if loads are deferred
	Super::StartInitialLoading();

	STARTUP_JOB(InitializeAbilitySystem());

	// Run all the queued up startup jobs
	DoAllStartupJobs();
}

void UKrakenAssetManager::DoAllStartupJobs()
{
	SCOPED_BOOT_TIMING("UKrakenAssetManager::DoAllStartupJobs");
	const double AllStartupJobsStartTime = FPlatformTime::Seconds();

	if (IsRunningDedicatedServer())
	{
		// No need for periodic progress updates, just run the jobs
		for (const FKrakenAssetManagerStartupJob& StartupJob : StartupJobs)
		{
			StartupJob.DoJob();
		}
	}
	else
	{
		if (StartupJobs.Num() > 0)
		{
			float TotalJobValue = 0.0f;
			for (const FKrakenAssetManagerStartupJob& StartupJob : StartupJobs)
			{
				TotalJobValue += StartupJob.JobWeight;
			}

			float AccumulatedJobValue = 0.0f;
			for (FKrakenAssetManagerStartupJob& StartupJob : StartupJobs)
			{
				const float JobValue = StartupJob.JobWeight;
				StartupJob.SubstepProgressDelegate.BindLambda([This = this, AccumulatedJobValue, JobValue, TotalJobValue](float NewProgress)
					{
						const float SubstepAdjustment = FMath::Clamp(NewProgress, 0.0f, 1.0f) * JobValue;
						const float OverallPercentWithSubstep = (AccumulatedJobValue + SubstepAdjustment) / TotalJobValue;

						This->UpdateInitialGameContentLoadPercent(OverallPercentWithSubstep);
					});

				StartupJob.DoJob();

				StartupJob.SubstepProgressDelegate.Unbind();

				AccumulatedJobValue += JobValue;

				UpdateInitialGameContentLoadPercent(AccumulatedJobValue / TotalJobValue);
			}
		}
		else
		{
			UpdateInitialGameContentLoadPercent(1.0f);
		}
	}

	StartupJobs.Empty();

	UE_LOG(LogTemp, Display, TEXT("All startup jobs took %.2f seconds to complete"), FPlatformTime::Seconds() - AllStartupJobsStartTime);
}

void UKrakenAssetManager::InitializeAbilitySystem()
{
	SCOPED_BOOT_TIMING("UKrakenAssetManager::InitializeAbilitySystem");

	FKrakenGameplayTags::InitializeNativeTags();

	// UAbilitySystemGlobals::Get().InitGlobalData();
}

void UKrakenAssetManager::UpdateInitialGameContentLoadPercent(float GameContentPercent)
{
	// Could route this to the early startup loading screen
}
