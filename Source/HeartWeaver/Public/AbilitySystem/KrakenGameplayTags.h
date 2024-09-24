// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"

class UGameplayTagsManager;

/**
 * FKrakenGameplayTags
 *
 *	Singleton containing native gameplay tags.
 */
struct FKrakenGameplayTags
{
	static const FKrakenGameplayTags& Get() { return GameplayTags; }

	static void InitializeNativeTags();

	static FGameplayTag FindTagByString(FString TagString, bool bMatchPartialString = false);

	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_AimMarkWithJoystick;
	FGameplayTag InputTag_Jump;
	FGameplayTag InputTag_Crouch;
	FGameplayTag InputTag_Climb;
	FGameplayTag Tag_Gameplay_AbilityInputBlocked;
	FGameplayTag Tag_Gameplay_MovementStopped;
	FGameplayTag Tag_Gameplay_MovementMust;

protected:

	void AddAllTags(UGameplayTagsManager& Manager);
	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);

private:

	static FKrakenGameplayTags GameplayTags;
};
