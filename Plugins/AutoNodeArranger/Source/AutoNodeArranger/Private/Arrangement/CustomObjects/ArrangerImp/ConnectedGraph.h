// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "_ExtendedObjects/ExtendedNode.h"
#include "_ExtendedObjects/ExtendedComment.h"

#define GROUP_ALL_CONNECTED_GRAPH_STEP

struct ConnectedGraph
{
	struct CommentRelativeInfo
	{
// ======== CONSTRUCTOR ========

		CommentRelativeInfo() {}

		CommentRelativeInfo(const ConnectedGraph& from, const ConnectedGraph& to)
		{
			intersectionSize = intersection(from.connectedCommentSet, to.connectedCommentSet).size();
			commentDiffCount = from.connectedCommentSet.size() + to.connectedCommentSet.size() - 2 * intersectionSize;
			commentHeaderCount = difference(to.connectedCommentSet, from.connectedCommentSet).size();
		}

// ======== ATTRIBUTES ========

		int intersectionSize = 0;
		int commentDiffCount = 0;
		int commentHeaderCount = 0;
	};

// ======== ATTRIBUTES ========

	std::set<ExtendedNode*> connectedNodeSet;
	std::set<ExtendedComment*> connectedCommentSet;
	ExtendedNode* globalRootNode = nullptr;

// ===== VARIABLE PROPERTIES =====

	bool isPlaced = false;

private:
	Box connectedGraphBox = Box::pos_size(Vector2(), Vector2());
	Box commentConnectedGraphBox = Box::pos_size(Vector2(), Vector2());

// ======== CONSTRUCTOR ========

public:
	ConnectedGraph(ExtendedNode* selectedNode, std::vector<ExtendedComment>& commentList)
	{
		selectedNode->UpdateConnectedNodeSet(connectedNodeSet, globalRootNode);
		for (auto connectedNode : connectedNodeSet)
			for (auto containedByIExtendedComment : connectedNode->containedByIExtendedCommentSet)
				connectedCommentSet.insert(&commentList[containedByIExtendedComment->getID()]); // other nodes contained by comment could be considered
	}

// ======== METHODS ========

	const Box* getConnectedGraphBox() { return &connectedGraphBox; }
	const Box* getCommentConnectedGraphBox() { return &commentConnectedGraphBox; }

	void updateConnectedGraphBox()
	{
		connectedGraphBox = globalRootNode->computeNodeBox();
		for (auto connectedNode : connectedNodeSet)
			connectedGraphBox += connectedNode->computeNodeBox();
	}

	void updateCommentConnectedGraphBox(const Vector2& commentSpacing)
	{
		commentConnectedGraphBox = globalRootNode->computeNodeBox();
		for (auto connectedNode : connectedNodeSet)
			commentConnectedGraphBox +=
				connectedNode->computeNodeBox().ComputeBoxToNotOverlapInX(connectedNode->containedByIExtendedCommentSet.size(),
					connectedNode->containedByIExtendedCommentSet.size(), Vector2(), Vector2(commentSpacing.x, -commentSpacing.y));
	}

	Vector2 getMinPos(const Vector2 &spacing, const Vector2 &commentSpacing) const
	{
		return connectedGraphBox.ComputeBoxToNotOverlapInX(0, connectedCommentSet.size(), spacing, commentSpacing).getMin();
	}

	void moveConnectedGraph(const Vector2& toAdd)
	{
		for (auto connectedNode : connectedNodeSet)
			connectedNode->SET_POS(GROUP_ALL_CONNECTED_GRAPH_STEP, connectedNode->pos + toAdd);
		connectedGraphBox = connectedGraphBox.offsetBox(toAdd);
		commentConnectedGraphBox = commentConnectedGraphBox.offsetBox(toAdd);
	}
};