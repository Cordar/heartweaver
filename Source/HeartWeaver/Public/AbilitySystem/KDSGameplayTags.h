// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"

class UGameplayTagsManager;

/**
 * FKDSGameplayTags
 *
 *	Singleton containing native gameplay tags.
 */
struct FKDSGameplayTags
{
public:

	static const FKDSGameplayTags& Get() { return GameplayTags; }

	static void InitializeNativeTags();

	static FGameplayTag FindTagByString(FString TagString, bool bMatchPartialString = false);

public:

	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Jump;
	FGameplayTag InputTag_Crouch;

protected:

	void AddAllTags(UGameplayTagsManager& Manager);
	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);

private:

	static FKDSGameplayTags GameplayTags;
};