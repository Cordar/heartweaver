// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "Debug.h"

FString int_to_tab_FString(int i)
{
	FString s;
	while (i--)
		s += "\t";
	return s;
}
