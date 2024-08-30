// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include <iostream>

#include "../Basic/Vector2.h"

struct RerouteInstruction
{
// ======== ATTRIBUTES ========

	Vector2 pos;

// ======== CONSTRUCTOR ========

	RerouteInstruction() : RerouteInstruction(Vector2()) {}
	explicit RerouteInstruction(Vector2 _pos) : pos(_pos) {}

// ======== READ/WRITE ========

	friend std::istream& operator>>(std::istream& is, RerouteInstruction& rerouteInstruction)
	{
		return is >> rerouteInstruction.pos;
	}

	friend std::ostream& operator<<(std::ostream& os, const RerouteInstruction& rerouteInstruction)
	{
		return os << rerouteInstruction.pos;
	}
};
