// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include <iostream>

struct LinkInstruction
{
// ======== ATTRIBUTES ========

	int fromPinID = -1;
	int toPinID = -1;

	bool bBreak = false;

	// ======== CONSTRUCTOR ========

	LinkInstruction() : LinkInstruction(-1, -1, false) {}
	LinkInstruction(int _fromPinID, int _toPinID, bool _bBreak) : fromPinID(_fromPinID), toPinID(_toPinID), bBreak(_bBreak) {}

// ======== READ/WRITE ========

	friend std::istream& operator>>(std::istream& is, LinkInstruction& linkInstruction)
	{
		return is >> linkInstruction.fromPinID >> linkInstruction.toPinID >> linkInstruction.bBreak;
	}

	friend std::ostream& operator<<(std::ostream& os, const LinkInstruction& linkInstruction)
	{
		return os << linkInstruction.fromPinID << " " << linkInstruction.toPinID << " " << linkInstruction.bBreak;
	}
};
