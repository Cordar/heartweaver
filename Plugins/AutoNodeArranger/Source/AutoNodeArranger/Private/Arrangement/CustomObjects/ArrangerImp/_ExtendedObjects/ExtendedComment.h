// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "../_CustomObjects/Graph/Comment.h"
#include "IExtendedComment.h"
#include "ExtendedNode.h"

struct ExtendedComment : CppComment, IExtendedComment
{
	using CppComment::CppComment;

// ======== ATTRIBUTES ========

	std::vector<ExtendedNode*> insideNodeList;
	std::vector<ExtendedComment*> insideCommentList;

	std::set<IExtendedComment*> containedByIExtendedCommentSet;

	std::set<ExtendedNode*> insideNodeSet;
	std::set<ExtendedNode*> leftNodeSet;
	std::set<ExtendedNode*> rightNodeSet;
	std::set<ExtendedNode*> tempInsideNodeSet;

private:
	double minPosX = -MAX_DOUBLE; // initialized in computeCommentExtension
	int64_t _placeOrder = -1;
	int placedNodeCount = 0;
	Box commentBox = Box::pos_size(Vector2(), Vector2());

// ======== ACommentAndHeaderDiffCounter METHODS ========

public:
	const std::set<const ACommentAndHeaderDiffCounter*> computeContainedByCommentSet() const override
	{
		std::set<const ACommentAndHeaderDiffCounter*> result;
		for (auto containedByIExtendedComment : containedByIExtendedCommentSet)
			result.insert(containedByIExtendedComment);
		result.insert(this);
		return result;
	}

// ======== IExtendedComment METHODS ========
public:

	int getID() const override
	{
		return ID;
	}

	void addNodeInside(ExtendedNode* insideNode) override
	{
		insideNodeIDList.push_back(insideNode->ID);
		insideNodeList.push_back(insideNode);
		insideNode->containedByIExtendedCommentSet.insert(this);
	}

	const std::set<ExtendedNode*>& getInsideNodeSet() const override
	{
		return insideNodeSet;
	}

	const std::set<ExtendedNode*>& getTempInsideNodeSet() const override
	{
		return tempInsideNodeSet;
	}

	double updateMinPosX(double newMinPosX, bool isMaterialOrSoundGraph) override
	{
		return minPosX = (minPosX < newMinPosX) != isMaterialOrSoundGraph ? newMinPosX : minPosX;
	}

	double getMinPosX() const override
	{
		return minPosX;
	}

	const int64_t& placeOrder() const override
	{
		return _placeOrder;
	}

	int64_t& placeOrder() override
	{
		return _placeOrder;
	}

	void increasePlacedNodeCount() override
	{
		placedNodeCount++;
	}

	bool isFullyPlaced() const override
	{
		return placedNodeCount == insideNodeSet.size();
	}

	void decreasePlacedNodeCount() override
	{
		placedNodeCount--;
	}

	bool isNonePlaced() const override
	{
		return placedNodeCount == 0;
	}

	void updateCommentBox(const Vector2& commentSpacing) override
	{
		if (insideNodeList.empty() && insideCommentList.empty())
		{
			commentBox = Box::pos_size(pos, size);
			return;
		}
		if (insideNodeList.empty())
		{
			insideCommentList[0]->updateCommentBox(commentSpacing);
			commentBox = insideCommentList[0]->getCommentBox();
		}
		else
			commentBox = insideNodeList[0]->computeNodeBox();
		for (auto insideNode : insideNodeList)
			commentBox += insideNode->computeNodeBox();
		for (auto insideComment : insideCommentList)
		{
			if (insideComment->getCommentBox().getSize() == Vector2())
				insideComment->updateCommentBox(commentSpacing);
			commentBox += insideComment->getCommentBox();
		}
		commentBox = commentBox.extendBox(commentSpacing);
	}

	const Box& getCommentBox() const override
	{
		return commentBox;
	}

	void ApplyFullyPlaced(Vector2 spacing, Vector2 commentSpacing, bool isAI_Graph,
		std::vector<ExtendedNode*>& placedNodeList, std::vector<IExtendedComment*>& placedCommentList) override
	{
		std::vector<IExtendedComment*> beforeCommentList;
		std::vector<IExtendedComment*> afterCommentList;
		for (auto placedComment : placedCommentList)
		{
			if (placedComment == this || contains(insideCommentList, placedComment))
				continue;
			(placedComment->placeOrder() < placeOrder() ? beforeCommentList : afterCommentList).push_back(placedComment);
		}
		std::vector<ExtendedNode*> beforeNodeList;
		std::vector<ExtendedNode*> afterNodeList;
		for (auto placedNode : placedNodeList)
		{
			if (contains(insideNodeList, placedNode))
				continue;
			bool isBeforeForAI = isAI_Graph && contains(leftNodeSet, placedNode);
			(isBeforeForAI || placedNode->getPlaceOrder() < placeOrder() ? beforeNodeList : afterNodeList).push_back(placedNode);
		}

		Vector2 toAdd;
		GetPosForNoOverlapCommentInY(toAdd, spacing, commentSpacing, beforeNodeList, beforeCommentList);
		if (toAdd.y != 0.)
		{
			for (auto insideNode : insideNodeSet)
				insideNode->SET_POS(PLACE_Y_COMMENT_NO_OVERLAP, insideNode->pos + toAdd);
			commentBox = commentBox.offsetBox(toAdd);
			for (auto insideComment : insideCommentList)
				insideComment->commentBox = insideComment->commentBox.offsetBox(toAdd);
		}

		for (auto afterNode : afterNodeList)
			afterNode->UndoPlaceInY(placedNodeList, placedCommentList);
		for (ExtendedNode* afterNode : afterNodeList)
			afterNode->PublicPlaceNodeInY(placedNodeList, placedCommentList);
	}

// ======== METHODS ========
public:

	virtual ~ExtendedComment() = default;

	void computeCommentExtensionForCommentAndNode(std::vector<ExtendedNode>& nodeList, std::vector<ExtendedComment>& commentList)
	{
		for (int insideNodeID : insideNodeIDList)
		{
			ExtendedNode& insideNode = nodeList[insideNodeID];
			insideNodeList.push_back(&insideNode);
			insideNode.containedByIExtendedCommentSet.insert(this);
		}
		for (int insideCommentID : insideCommentIDList)
		{
			ExtendedComment& insideComment = commentList[insideCommentID];
			insideCommentList.push_back(&insideComment);
			insideComment.containedByIExtendedCommentSet.insert(this);
		}
	}

	// computeNodeExtension must be called on all nodes before
	void computeCommentExtension(bool isAI_graph, bool isMaterialOrSoundGraph)
	{
		minPosX = isMaterialOrSoundGraph ? MAX_DOUBLE : -MAX_DOUBLE;
		UpdateDirectionNodeSet(isAI_graph);
	}

private:

	void UpdateDirectionNodeSet(bool isAI_graph)
	{
		insideNodeSet = std::set<ExtendedNode*>(insideNodeList.begin(), insideNodeList.end());

		for (auto insideNode : insideNodeSet)
		{
			UpdateLeftNodeSet(insideNode); // insideNode removed later from leftNodeSet
			UpdateRightNodeSet(insideNode); // insideNode removed later from rightNodeSet
		}

		leftNodeSet = difference(leftNodeSet, insideNodeSet);
		rightNodeSet = difference(rightNodeSet, insideNodeSet);
		tempInsideNodeSet = intersection(leftNodeSet, rightNodeSet);
		leftNodeSet = difference(leftNodeSet, tempInsideNodeSet);
		rightNodeSet = difference(rightNodeSet, tempInsideNodeSet);

		for (auto leftNode : leftNodeSet)
			updateAccessibleExecNodeSet(leftNode, leftNodeSet); // exec on left
		for (auto rightNode : rightNodeSet)
			updateAccessibleNodeSet(rightNode, rightNodeSet); // all accessible on right
		for (auto leftNode : leftNodeSet)
			updateAccessibleNodeSet(leftNode, leftNodeSet); // remaining on left

		if (isAI_graph) // force inclusion of temp inside nodes for AI graph
		{
			for (auto tempInsideNode : tempInsideNodeSet)
				addNodeInside(tempInsideNode);
			tempInsideNodeSet.clear();
		}
	}

	void UpdateLeftNodeSet(ExtendedNode* currentNode)
	{
		if (contains(leftNodeSet, currentNode))
			return;
		leftNodeSet.insert(currentNode);
		for (auto leftConnectionList : currentNode->leftConnectionListArray)
			for (auto leftConnection : *leftConnectionList)
				UpdateLeftNodeSet(leftConnection.connectedToNode);
	}

	void UpdateRightNodeSet(ExtendedNode* currentNode)
	{
		if (contains(rightNodeSet, currentNode))
			return;
		rightNodeSet.insert(currentNode);
		for (auto rightConnectionList : currentNode->rightConnectionListArray)
			for (auto rightConnection : *rightConnectionList)
				UpdateRightNodeSet(rightConnection.connectedToNode);
	}

	void updateAccessibleExecNodeSet(ExtendedNode* currentNode, std::set<ExtendedNode*>& accessibleNodeSet)
	{
		accessibleNodeSet.insert(currentNode);
		for (auto execConnectionList : currentNode->execConnectionListArray)
		{
			for (auto execConnection : *execConnectionList)
			{
				auto nextNode = execConnection.connectedToNode;
				if (contains(rightNodeSet, nextNode) || contains(leftNodeSet, nextNode) ||
					contains(insideNodeSet, nextNode) || contains(tempInsideNodeSet, nextNode))
					continue;
				updateAccessibleExecNodeSet(nextNode, accessibleNodeSet);
			}
		}
	}

	void updateAccessibleNodeSet(ExtendedNode* currentNode, std::set<ExtendedNode*>& accessibleNodeSet)
	{
		accessibleNodeSet.insert(currentNode);
		for (auto allConnectionList : currentNode->allConnectionListArray)
		{
			for (auto connection : *allConnectionList)
			{
				auto nextNode = connection.connectedToNode;
				if (contains(rightNodeSet, nextNode) || contains(leftNodeSet, nextNode) ||
					contains(insideNodeSet, nextNode) || contains(tempInsideNodeSet, nextNode))
					continue;
				updateAccessibleNodeSet(nextNode, accessibleNodeSet);
			}
		}
	}

// ======== ARRANGEMENT METHODS ========

	void GetPosForNoOverlapCommentInY(Vector2& toAdd, Vector2 spacing, Vector2 commentSpacing,
		const std::vector<ExtendedNode*>& placedNodeList, const std::vector<IExtendedComment*>& placedCommentList)
	{
		const Box& box = getCommentBox();
		Vector2 halfSpacingY = Vector2(0., 0.5 * spacing.y);
		bool hasIntersection = true;
		while (hasIntersection)
		{
			hasIntersection = false;
			for (auto placedComment : placedCommentList)
			{
				if (placedComment == this || contains(containedByIExtendedCommentSet, placedComment)
					|| getOrComputeCommentDiffCount(placedComment) > 2)
					continue;
				const Box commBox = extendBoxWithComment(box, placedComment, commentSpacing).offsetBox(toAdd);
				const Box placedCommentBox = placedComment->extendBoxWithComment(placedComment->getCommentBox(), this, commentSpacing)
					.extendBox(halfSpacingY).offsetBox(halfSpacingY);
				if (commBox.IntersectWith(placedCommentBox))
				{
					hasIntersection = true;
					toAdd.y += placedCommentBox.getMax().y - commBox.getMin().y + 1;
				}
			}
			for (auto placedNode : placedNodeList)
			{
				if (contains(insideNodeSet, placedNode) || getOrComputeCommentDiffCount(placedNode) > 1)
					continue;
				const Box commBox = extendBoxWithComment(box, placedNode, commentSpacing).offsetBox(toAdd).extendBox(-COMMENT_HEADER);
				const Box placedNodeBox = placedNode->extendBoxWithComment(placedNode->computeNodeBox(), this, commentSpacing)
					.extendBox(halfSpacingY).offsetBox(halfSpacingY);
				if (commBox.IntersectWith(placedNodeBox))
				{
					hasIntersection = true;
					toAdd.y += placedNodeBox.getMax().y - commBox.getMin().y + 1;
				}
			}
		}
	}
};
