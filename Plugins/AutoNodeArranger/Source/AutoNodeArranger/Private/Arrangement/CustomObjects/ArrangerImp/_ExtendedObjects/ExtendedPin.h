// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "../_CustomObjects/Graph/Pin.h"

struct ExtendedPin : CppPin
{
	using CppPin::CppPin;

// ======== ATTRIBUTES ========

	int owningNodeID = 0;

// ======== METHODS ========

	void computePinExtension(int _owningNodeID)
	{
		owningNodeID = _owningNodeID;
	}
};
