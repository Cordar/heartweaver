// Copyright 2022 bstt, Inc. All Rights Reserved.

#include "ANA_ReleaseNotifier.h"
#include "../Helpers/ANA_CacheHelper.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "../Debug/Log.h"
#include "Subsystems/AssetEditorSubsystem.h"

#include "Debug/Debug.h"

#define ANA_NEW_RELEASE_VERSION 414

TWeakPtr<SNotificationItem> ANA_ReleaseNotifier::releaseNotifPtr;
CompleteActionType ANA_ReleaseNotifier::completeActionType = CompleteActionType::NONE;

void ANA_ReleaseNotifier::RegisterCheckVersion()
{
	static FDelegateHandle onAssetEditorOpenedHandle;

	if (UAssetEditorSubsystem* assetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
		onAssetEditorOpenedHandle = assetEditorSubsystem->OnAssetEditorOpened().AddLambda(
			[assetEditorSubsystem](UObject* Object)
			{
				ANA_ReleaseNotifier::CheckVersion();
				assetEditorSubsystem->OnAssetEditorOpened().Remove(onAssetEditorOpenedHandle);
			});
}

void ANA_ReleaseNotifier::CheckVersion()
{
	FANA_CacheData cacheData = ANA_CacheHelper::GetCacheData();

	if (cacheData.version >= ANA_NEW_RELEASE_VERSION)
		return;
	if (cacheData.laterTime >= FDateTime::UtcNow())
		return;
	ShowNotification(CompleteActionType::NONE);
}

void ANA_ReleaseNotifier::ShowNotification(CompleteActionType completeActionType_)
{
	completeActionType = completeActionType_;

	switch (completeActionType)
	{
	case NONE:
		break;
	default:
		break;
	}
}
