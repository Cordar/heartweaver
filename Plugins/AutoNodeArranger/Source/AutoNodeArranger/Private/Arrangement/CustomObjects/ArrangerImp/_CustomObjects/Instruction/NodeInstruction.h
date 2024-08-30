// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "../Basic/Vector2.h"

struct NodeInstruction
{
// ======== ATTRIBUTES ========

	int nodeID = -1;

	Vector2 fromPos;
	Vector2 toPos;

// ======== CONSTRUCTOR ========

	NodeInstruction() : NodeInstruction(-1, Vector2(), Vector2()) {}
	NodeInstruction(int _nodeID, Vector2 _fromPos, Vector2 _toPos) : nodeID(_nodeID), fromPos(_fromPos), toPos(_toPos) {}

// ======== READ/WRITE ========

	friend std::istream& operator>>(std::istream& is, NodeInstruction& nodeInstruction)
	{
		return is >> nodeInstruction.nodeID >> nodeInstruction.fromPos >> nodeInstruction.toPos;
	}

	friend std::ostream& operator<<(std::ostream& os, const NodeInstruction& nodeInstruction)
	{
		return os << nodeInstruction.nodeID << " " << nodeInstruction.fromPos << " " << nodeInstruction.toPos;
	}
};
