// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include <vector>
#include <set>
#include "ACommentAndHeaderDiffCounter.h"

struct Vector2;
struct ExtendedNode;

struct IExtendedComment : ACommentAndHeaderDiffCounter
{
// ======== METHODS ========

 // called by ConnectedGraph

	virtual int getID() const = 0;

 // called by LoopConnectionLib

	virtual void addNodeInside(ExtendedNode* insideNode) = 0;

 // called by ExtendedNode

	virtual const std::set<ExtendedNode*>& getInsideNodeSet() const = 0;
	virtual const std::set<ExtendedNode*>& getTempInsideNodeSet() const = 0;

	virtual double updateMinPosX(double newMinPosX, bool isMaterialOrSoundGraph) = 0;
	virtual double getMinPosX() const = 0;
	virtual const int64_t& placeOrder() const = 0;
	virtual int64_t& placeOrder() = 0;
	virtual void increasePlacedNodeCount() = 0;
	virtual bool isFullyPlaced() const = 0;
	virtual void decreasePlacedNodeCount() = 0;
	virtual bool isNonePlaced() const = 0;
	virtual void updateCommentBox(const Vector2& commentSpacing) = 0;
	virtual const Box& getCommentBox() const = 0;
	virtual void ApplyFullyPlaced(Vector2 spacing, Vector2 commentSpacing, bool isAI_Graph,
		std::vector<ExtendedNode*>& placedNodeList, std::vector<IExtendedComment*>& placedCommentList) = 0;
};