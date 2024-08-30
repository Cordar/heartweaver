// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

enum CompleteActionType
{
	NONE,
};

/**
 * 
 */
class ANA_ReleaseNotifier
{
	ANA_ReleaseNotifier() = delete;
public:
	static void RegisterCheckVersion();
	static void CheckVersion();
	static void ShowNotification(CompleteActionType completedActionType_);

private:
	static void ShowPollNotification();
	
	static TWeakPtr<SNotificationItem> releaseNotifPtr;
	static CompleteActionType completeActionType;
};
