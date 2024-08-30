// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <map>
#include <set>
#include <queue>

#include "../_CustomObjects/Basic/Box.h"
#include "../_CustomObjects/Graph/Node.h"
#include "../_CustomObjects/Graph/GraphConfig.h"
#include "../_CustomObjects/Instruction/LinkInstruction.h"
#include "../_CustomObjects/Instruction/NodeInstruction.h"
#include "ExtendedPin.h"
#include "IExtendedComment.h"

#include "ExtendedCommon.h"

#include "../CDebug.h"

// requires USE_EXE
#define C_DEBUG_MOD true

#define PLACE_X_ON_LEFT
#define PLACE_X_ON_RIGHT
#define PLACE_X_REROUTE_LOOP
#define PLACE_Y_NO_NODE_OVERLAP
#define PLACE_Y_NO_CONNECTION_OVERLAP
#define PLACE_Y_COMMENT_NO_OVERLAP
#define PLACE_Y_NO_COMMENT_OVERLAP
#define PLACE_Y_REROUTE_LOOP
#define PLACE_OFFSET

#define AI_PLACE_X
#define AI_PLACE_OFFSET

#define PLACE_CENTERIZED

#define SCALE_LINE_WIDTH 5.
#define ALIGN_THRESHOLD 10.

#define MAX_LOOP_COUNT 5

#define MAX_DOUBLE 100000.

#define SET_POS(step, toPos) SetPos(toPos, #step, std::string(__FILE__), __LINE__, __FUNCTION__)

struct ExtendedNode : CppNode, ACommentAndHeaderDiffCounter
{
	using CppNode::CppNode;

	struct Connection
	{
// ======== ATTRIBUTES ========

		bool bLeft = false;
		bool bExec = false;
		bool bCompact = false;

		ExtendedPin* connectedFromPin = nullptr;
		ExtendedPin* connectedToPin = nullptr;

		ExtendedNode* connectedFromNode = nullptr;
		ExtendedNode* connectedToNode = nullptr;

		Vector2 deltaPos;

// ======== CONSTRUCTOR ========

		Connection() : Connection(false, false, false, nullptr, nullptr, nullptr, nullptr, Vector2()) {}

		Connection(bool _bLeft, bool _bExec, bool _bCompact, ExtendedPin* _connectedFromPin, ExtendedPin* _connectedToPin,
			ExtendedNode* _connectedFromNode, ExtendedNode* _connectedToNode, Vector2 _deltaPos) :
			bLeft(_bLeft), bExec(_bExec), bCompact(_bCompact), connectedFromPin(_connectedFromPin), connectedToPin(_connectedToPin),
			connectedFromNode(_connectedFromNode), connectedToNode(_connectedToNode), deltaPos(_deltaPos) {}

// ======== METHODS ========

		LinkInstruction breakConnection()
		{
			connectedFromPin->connectedToPinIDList.erase(
				find(connectedFromPin->connectedToPinIDList.begin(),
					connectedFromPin->connectedToPinIDList.end(), connectedToPin->ID));
			connectedToPin->connectedToPinIDList.erase(
				find(connectedToPin->connectedToPinIDList.begin(),
					connectedToPin->connectedToPinIDList.end(), connectedFromPin->ID));

			auto& connectedToExtendedConnectionList = connectedToNode->touchExtendedConnectionList(!bLeft, bExec);
			connectedToExtendedConnectionList.erase(GetOppositeConnection());

			auto& connectedFromExtendedConnectionList = connectedFromNode->touchExtendedConnectionList(bLeft, bExec);
			connectedFromExtendedConnectionList.erase(
				std::find(connectedFromExtendedConnectionList.begin(), connectedFromExtendedConnectionList.end(), *this));

			return LinkInstruction(connectedFromPin->ID, connectedToPin->ID, true);
		}

		friend bool operator==(const Connection& lhs, const Connection& rhs)
		{
			return lhs.connectedFromPin == rhs.connectedFromPin && lhs.connectedToPin == rhs.connectedToPin;
		}

		std::string connectionToString() const
		{
			return connectedFromNode->name + " : " + "[" + connectedFromPin->name + " -> " + connectedToPin->name + "]";
		}

		typedef bool (*ptr_comparator)(const Connection*);

		static ptr_comparator ptr_equal_op(const Connection* lhs)
		{
			static const Connection* static_lhs;
			static_lhs = lhs;
			return [](const Connection* rhs) { return *static_lhs == *rhs; };
		}

		// find constructed opposite connection on correponding connectedTo connectionList
		std::vector<Connection>::iterator GetOppositeConnection()
		{
			auto& connectedToExtendedConnectionList = connectedToNode->touchExtendedConnectionList(!bLeft, bExec);
			auto connectionToFindOpposite = ConstructOppositeConnection();
			return std::find(connectedToExtendedConnectionList.begin(), connectedToExtendedConnectionList.end(), connectionToFindOpposite);
		}

		Box ComputeConnectionBox(bool& isConnectionAligned) const
		{
			ExtendedNode* leftNode = connectedFromNode;
			ExtendedNode* rightNode = connectedToNode;
			Vector2 leftPinOffset = connectedFromPin->offset;
			Vector2 rightPinOffset = connectedToPin->offset;
			if ((bLeft) != graphConfig->GetIsMaterialOrSoundGraph())
			{
				leftNode = connectedToNode;
				rightNode = connectedFromNode;
				leftPinOffset = connectedToPin->offset;
				rightPinOffset = connectedFromPin->offset;
			}
			Vector2 leftPinTargetPos = leftNode->pos + leftPinOffset;
			Vector2 rightPinTargetPos = rightNode->pos + Vector2(-rightPinOffset.x, rightPinOffset.y);
			isConnectionAligned = std::fabs(leftPinTargetPos.y - rightPinTargetPos.y) <= ALIGN_THRESHOLD;
			return Box::pos_size(Vector2(leftPinTargetPos.x, leftPinTargetPos.y - 0.5 * graphConfig->lineWidth),
					Vector2(rightPinTargetPos.x - leftPinTargetPos.x, graphConfig->lineWidth));
		}

	private:
		// deltaPos not computed
		Connection ConstructOppositeConnection()
		{
			return Connection(!bLeft, bExec, bCompact, connectedToPin, connectedFromPin, connectedToNode, connectedFromNode, Vector2());
		}

	};
// ======== ATTRIBUTES ========

	int leftNonExecConnectionCount = 0;
	int rightNonExecConnectionCount = 0;
	bool isRerouteNodeOneOutput = false;

	std::vector<Connection> leftExecConnectionList;
	std::vector<Connection> leftNonExecConnectionList;
	std::vector<Connection> rightExecConnectionList;
	std::vector<Connection> rightNonExecConnectionList;

	std::set<IExtendedComment*> containedByIExtendedCommentSet;

	std::array<std::vector<Connection>*, 4> allConnectionListArray;
	std::array<std::vector<Connection>*, 2> execConnectionListArray;
	std::array<std::vector<Connection>*, 2> leftConnectionListArray;
	std::array<std::vector<Connection>*, 2> rightConnectionListArray;

	// static attributes
	static std::vector<NodeInstruction>* nodeInstructionList;
	static CppGraphConfig* graphConfig;

	// Reroute attributes
	ExtendedNode* rerouteLoopNodeParent = nullptr;
	ExtendedNode* rerouteLoopNodeChild = nullptr;
	ExtendedNode* rerouteLoopNodeBrother = nullptr;
	bool isRerouteLoopNodePlacedOnRight = false;

	// Centerize attributes
	std::vector<ExtendedNode*> forCenterizeToUpdateNodeList;
	int remainingCenterizeNodeToPlaceCount = -1;

private:

	struct PlaceInYParam
	{
		PlaceInYParam() {}
		PlaceInYParam(
			const std::vector<ExtendedNode*>& _placedNodeList,
			const std::vector<IExtendedComment*>& _placedCommentList,
			const std::set<Connection*>& _placedConnectionSet,
			const std::vector<ExtendedNode*>& _nodeStack) :
				placedNodeList(_placedNodeList),
				placedCommentList(_placedCommentList),
				placedConnectionSet(_placedConnectionSet),
				nodeStack(_nodeStack) {}

		std::vector<ExtendedNode*> placedNodeList;
		std::vector<IExtendedComment*> placedCommentList;
		std::set<Connection*> placedConnectionSet;
		std::vector<ExtendedNode*> nodeStack;
	};

// ===== VARIABLE PROPERTIES =====

private:
	bool GetIsVisited() const { return bVisited == *bVisitedValue; }
	void SetIsVisited() { bVisited = *bVisitedValue; }
	void SetIsNotVisited() { bVisited = !*bVisitedValue; }

	// Reverse the visited state of all nodes
	static void ToggleVisitedValue() { *bVisitedValue = !*bVisitedValue; }

private:
	static bool* bVisitedValue;
	bool bVisited = false;
	bool isToPlaceOnRight = true;
	int64_t placeOrder = -1; // y place order

	// place in y reference --> needed to place again
	ExtendedNode* refNode = nullptr;
	bool refExec;

// ======== ACommentAndHeaderDiffCounter METHODS ========

public:
	const std::set<const ACommentAndHeaderDiffCounter*> computeContainedByCommentSet() const override
	{
		std::set<const ACommentAndHeaderDiffCounter*> result;
		for (auto containedByIExtendedComment : containedByIExtendedCommentSet)
			result.insert(containedByIExtendedComment);
		return result;
	}

// ======== METHODS ========

public:
	int64_t getPlaceOrder() const { return placeOrder; }

	void updateConnectionListArray()
	{
		allConnectionListArray = { { &leftExecConnectionList, &leftNonExecConnectionList, &rightExecConnectionList, &rightNonExecConnectionList } };
		execConnectionListArray = { { &leftExecConnectionList, &rightExecConnectionList } };
		leftConnectionListArray = { { &leftExecConnectionList, &leftNonExecConnectionList } };
		rightConnectionListArray = { { &rightExecConnectionList, &rightNonExecConnectionList } };
	}

	static void setNodeStaticAttributes(std::vector<NodeInstruction>* _nodeInstructionList, CppGraphConfig* _graphConfig)
	{
		nodeInstructionList = _nodeInstructionList;
		graphConfig = _graphConfig;
		static bool static_bVisitedValue = true;
		bVisitedValue = &static_bVisitedValue;
	}

	void computeNodeExtensionForPin(std::vector<ExtendedPin>& pinList)
	{
		for (auto pinIDList : pinIDListArray)
			for (auto& pinID : *pinIDList)
				pinList[pinID].computePinExtension(ID);
		for (auto leftNonExecPinID : leftNonExecPinIDList)
			leftNonExecConnectionCount += pinList[leftNonExecPinID].connectedToPinIDList.size();
		for (auto rightNonExecPinID : rightNonExecPinIDList)
			rightNonExecConnectionCount += pinList[rightNonExecPinID].connectedToPinIDList.size();
	}

	// computePinExtensionForNode must be called on all nodes before
	void computeNodeExtension(std::vector<ExtendedPin>& pinList, std::vector<ExtendedNode>& nodeList)
	{
		for (int i = 0; i < pinIDListArray.size(); ++i)
		{
			for (auto& connectedFromPinID : *pinIDListArray[i])
			{
				auto& connectedFromPin = pinList[connectedFromPinID];
				for (auto connectedToPinID : connectedFromPin.connectedToPinIDList)
				{
					auto& connectedToPin = pinList[connectedToPinID];
					auto& connectedToNode = nodeList[connectedToPin.owningNodeID];
					bool isLeft = (i < 2);
					bool isExec = (i % 2 == 0);
					float spacingX = (isExec ? graphConfig->execSpacingX : graphConfig->spacing.x);
					int commentDiffCount = getOrComputeCommentDiffCount(&connectedToNode);
					bool isCompacted = graphConfig->bCompact && graphConfig->GetIsBlueprintGraph() && !isExec
						&& (isLeft ? leftNonExecConnectionCount == 1 && connectedToNode.rightNonExecConnectionCount == 1 :
							connectedToNode.leftNonExecConnectionCount == 1 && rightNonExecConnectionCount == 1);
					double sizeX = (graphConfig->GetIsMaterialOrSoundGraph() != isLeft) ? connectedToNode.size.x : size.x;
					allConnectionListArray[i]->emplace_back(
						isLeft,
						isExec,
						isCompacted,
						&connectedFromPin,
						&connectedToPin,
						this,
						&connectedToNode,
						isCompacted ?
							Vector2(0., isLeft ?
								(size.y + graphConfig->spacing.y + commentDiffCount * graphConfig->commentSpacing.y) :
									(- (connectedToNode.size.y + graphConfig->spacing.y + commentDiffCount * graphConfig->commentSpacing.y))) :
						graphConfig->GetIsAI_Graph() ?
							Vector2(0.5 * (connectedToNode.size.x - size.x) * (isLeft ? 1. : -1.), isLeft ?
								(- (connectedToNode.size.y + graphConfig->spacing.y + commentDiffCount * graphConfig->commentSpacing.y)) :
										(size.y + graphConfig->spacing.y + commentDiffCount * graphConfig->commentSpacing.y)) :
							Vector2(
								(((graphConfig->GetIsMaterialOrSoundGraph() != isLeft) ? -1. : 1.) *
		 							(sizeX + spacingX + commentDiffCount * graphConfig->commentSpacing.x)),
								connectedFromPin.offset.y - connectedToPin.offset.y)
						);
				}
			}
		}
		isRerouteNodeOneOutput = isRerouteNode && (rightExecConnectionList.size() + rightNonExecConnectionList.size() == 1);
	}

	void computeCompactNodeExtension()
	{
		for (int i = 0; i < allConnectionListArray.size(); ++i)
		{
			bool isLeft = (i < 2);
			bool isExec = (i % 2 == 0);
			float spacingX = (isExec ? graphConfig->execSpacingX : graphConfig->spacing.x);
			for (auto& connection : *allConnectionListArray[i])
			{
				if (connection.bCompact)
					continue; 
				int commentDiffCount = getOrComputeCommentDiffCount(connection.connectedToNode);
				ExtendedNode* currentNode = isLeft ? connection.connectedToNode : this;
				double maxSizeX = currentNode->size.x;
				while (currentNode)
				{
					maxSizeX = std::max(maxSizeX, currentNode->size.x);
					currentNode = currentNode->getNextCompactedNode();
				}
				connection.deltaPos =
					Vector2(
						(isLeft ? -1. : 1.) * (maxSizeX + spacingX + commentDiffCount * graphConfig->commentSpacing.x),
							connection.connectedFromPin->offset.y - connection.connectedToPin->offset.y);
			}
		}
	}
	
	Vector2 computeDeltaPosForRerotueNode(bool isInputConnection)
	{
		double deltaPosY = rerouteLoopNodeParent->size.y + graphConfig->spacing.y +
			getOrComputeHeaderCount(rerouteLoopNodeParent) * COMMENT_HEADER.y +
			getOrComputeCommentDiffCount(rerouteLoopNodeParent) * graphConfig->commentSpacing.y;
		return isInputConnection ? Vector2(size.x - rerouteLoopNodeParent->size.x, - deltaPosY) : Vector2(0., deltaPosY);
	}

	// Add all connected nodes to connectedNodeSet and set the globalRootNode
	void UpdateConnectedNodeSet(std::set<ExtendedNode*>& connectedNodeSet, ExtendedNode*& globalRootNode)
	{
		if (contains(connectedNodeSet, this))
			return;
		connectedNodeSet.insert(this);
		if (leftExecConnectionList.empty())
		{
			/*
			better to change if
			- current is nullptr
			- this is more rightExecConnected
			- this is more on top
			- this is more on left
			*/
			bool isBetterToChange = globalRootNode == nullptr ? true :
				globalRootNode->rightExecConnectionList.empty() != rightExecConnectionList.empty() ? globalRootNode->rightExecConnectionList.empty() :
				globalRootNode->pos.y != pos.y ? globalRootNode->pos.y > pos.y :
				(graphConfig->GetIsMaterialOrSoundGraph() != globalRootNode->pos.x > pos.x);
			if (isBetterToChange)
				globalRootNode = this;
		}
		for (auto allConnectionList : allConnectionListArray)
			for (auto allConnection : *allConnectionList)
				allConnection.connectedToNode->UpdateConnectedNodeSet(connectedNodeSet, globalRootNode);
	}

private:
	// computeNodeExtension must be called
	ExtendedNode* getNextCompactedNode()
	{
		return leftNonExecConnectionList.empty() || !leftNonExecConnectionList[0].bCompact ? nullptr : leftNonExecConnectionList[0].connectedToNode;
	}

	bool GetIsExecConnectedAtLeft()
	{
		if (GetIsVisited())
			return true;
		for (auto& leftExecConnection : leftExecConnectionList)
		{
			auto leftNode = leftExecConnection.connectedToNode;
			if (leftNode->rerouteLoopNodeParent)
				continue;
			if (leftNode->GetIsExecConnectedAtLeft())
				return true;
		}
		return false;
	}

	bool GetWillBePlacedAtRight()
	{
		// name should be GetIsNonExecConnectedAtRightToAnExecConnectedAtLeft
		if (GetIsExecConnectedAtLeft())
			return true;
		for (auto rightNonExecConnection : rightNonExecConnectionList)
		{
			if (rightNonExecConnection.connectedToNode->GetWillBePlacedAtRight())
				return true;
		}
		return false;
	}

// ======== UTILS METHODS ========

public:

	void SetPos(const Vector2& toPos, const char* stepName, std::string filename, int line, const char* functionName)
	{
		if (toPos == pos)
			return;
#if C_DEBUG_MOD
		static int i = 0;
		std::cout << filename.substr(filename.find_last_of("/\\") + 1) << "::" << line << "::" << functionName << " : "
			<< i << " " << stepName << " " << name << " " << pos << " -> " << toPos << std::endl;
		i++;
#endif
		nodeInstructionList->emplace_back(ID, pos, toPos);
		pos = toPos;
	}

	Box computeNodeBox() const
	{
		return Box::pos_size(pos, size);
	}

	std::vector<Connection>& touchExtendedConnectionList(bool bLeft, bool bExec)
	{
		return (bLeft ?
			bExec ? leftExecConnectionList : leftNonExecConnectionList :
			bExec ? rightExecConnectionList : rightNonExecConnectionList);
	}

private:

	std::set<Connection*> ComputeConnectionSetToAppend(const std::vector<ExtendedNode*>& placedNodeList) const
	{
		std::set<Connection*> result;
		for (auto allConnectionList : allConnectionListArray)
		{
			for (auto& connection : *allConnectionList)
			{
				if (contains(placedNodeList, connection.connectedToNode))
					result.insert(&connection);
			}
		}
		return result;
	}

// ======== ARRANGEMENT METHODS ========

public:
	// method to call only on globalRootNode
	void PlaceAllNodes()
	{
		Vector2 globalRootNodePos = pos;
		if (GetIsVisited()) // this should never happen
			ToggleVisitedValue();

		if (graphConfig->GetIsAI_Graph())
		{
			AI_PlaceInX();
			ToggleVisitedValue();
			AI_PlaceInY();
		}
		else
		{
			PlaceInX();
			ToggleVisitedValue();
			PlaceInY();
		}
		ToggleVisitedValue();
		// ensure globalRootNode do not move
		offsetAll(globalRootNodePos - pos);
	}

	// all nodes on right exec and on right and left non exec
	void GetOnRightExecVisitedNodeSet(std::set<ExtendedNode*>& onRightExecVisitedNodeSet)
	{
		if (!GetIsVisited() || contains(onRightExecVisitedNodeSet, this))
			return;
		onRightExecVisitedNodeSet.insert(this);
		for (auto connectionList : { &rightExecConnectionList, &leftNonExecConnectionList, &rightNonExecConnectionList })
			for (auto& connection : *connectionList)
				connection.connectedToNode->GetOnRightExecVisitedNodeSet(onRightExecVisitedNodeSet);
	}

	// called by ExtendedComment in ApplyFullyPlaced
	void UpdatePosForNoOverlapNodeInY(const std::vector<ExtendedNode*>& placedNodeList, const std::vector<IExtendedComment*>& placedCommentList)
	{
		double minPosY = pos.y;
		GetPosForNoOverlapNodeInY(minPosY, placedNodeList, placedCommentList);
		SET_POS(PLACE_Y_NO_COMMENT_OVERLAP, Vector2(pos.x, minPosY));
	}

	// called by ExtendedComment in ApplyFullyPlaced
	void UndoPlaceInY(std::vector<ExtendedNode*>& placedNodeList, std::vector<IExtendedComment*>& placedCommentList)
	{
		SetIsNotVisited();
		placedNodeList.erase(std::find(placedNodeList.begin(), placedNodeList.end(), this));
		for (auto containedByIExtendedComment : containedByIExtendedCommentSet)
		{
			if (containedByIExtendedComment->isFullyPlaced())
				placedCommentList.erase(std::find(placedCommentList.begin(), placedCommentList.end(), containedByIExtendedComment));
			containedByIExtendedComment->decreasePlacedNodeCount();
			if (containedByIExtendedComment->isNonePlaced())
				containedByIExtendedComment->placeOrder() = -1;
		}
	}

	void PublicPlaceNodeInY(std::vector<ExtendedNode*>& placedNodeList, std::vector<IExtendedComment*>& placedCommentList)
	{
		PlaceInYParam tmpPlaceInYParam;
		tmpPlaceInYParam.placedNodeList = placedNodeList;
		tmpPlaceInYParam.placedCommentList = placedCommentList;
		if (refNode)
		{
			if (refExec)
				refNode->PlaceExecAtRightInY(tmpPlaceInYParam);
			else
				refNode->PlaceNonExecAtRightInY(tmpPlaceInYParam);
		}
		placedNodeList = tmpPlaceInYParam.placedNodeList;
		placedCommentList = tmpPlaceInYParam.placedCommentList;
	}

// ======== ARRANGEMENT IN X ========

private:
	void PlaceInX()
	{
		std::queue<ExtendedNode*> toPlaceLaterNodeQueue;
		PlaceWithQueueInX(toPlaceLaterNodeQueue);
		while (!toPlaceLaterNodeQueue.empty())
		{
			toPlaceLaterNodeQueue.front()->PlaceAtRightInX(toPlaceLaterNodeQueue);
			toPlaceLaterNodeQueue.pop();
		}
	}
	
	void PlaceWithQueueInX(std::queue<ExtendedNode*>& toPlaceLaterNodeQueue)
	{
		if (GetIsVisited())
			return;
		PlaceAtLeftInX(toPlaceLaterNodeQueue);
		PlaceAtRightInX(toPlaceLaterNodeQueue);
	}

	void PlaceAtLeftInX(std::queue<ExtendedNode*>& toPlaceLaterNodeQueue)
	{
		std::vector<ExtendedNode*> notFreeNodeAtLeftList;
		std::set<ExtendedNode*> visitedNodeSet;
		GetNotFreeNodeAtLeftList(notFreeNodeAtLeftList, false, visitedNodeSet);

		PlacePlaceableOnLeftInX(notFreeNodeAtLeftList);
		PlacePlaceableOnRightInX(visitedNodeSet);

		for (auto visitedNode : visitedNodeSet)
			toPlaceLaterNodeQueue.push(visitedNode);
	}

	void PlaceAtRightInX(std::queue<ExtendedNode*>& toPlaceLaterNodeQueue)
	{
		for (auto rightConnectionList : rightConnectionListArray)
			for (auto rightConnection : *rightConnectionList)
				if (rightConnection.connectedToNode->rerouteLoopNodeParent)
					rightConnection.connectedToNode->SetRerouteLoopNodeVisitedInX();
				else
					rightConnection.connectedToNode->PlaceWithQueueInX(toPlaceLaterNodeQueue);
	}

	void PlacePlaceableOnLeftInX(const std::vector<ExtendedNode*>& notFreeNodeAtLeftList)
	{
		for (auto notFreeNodeAtLeft : notFreeNodeAtLeftList)
			notFreeNodeAtLeft->SetIsNotVisited();
		for (auto notFreeNodeAtLeft : notFreeNodeAtLeftList)
			notFreeNodeAtLeft->PlaceOnLeftInX(notFreeNodeAtLeftList);
	}

	void PlacePlaceableOnRightInX(const std::set<ExtendedNode*>& visitedNodeSet)
	{
		std::vector<ExtendedNode*> visitedNodeList = std::vector<ExtendedNode*>(visitedNodeSet.begin(), visitedNodeSet.end());
		for (auto visitedNode : visitedNodeSet)
			visitedNode->PlaceOnRightInX(visitedNodeList);
		std::vector<ExtendedNode*> nodeToPlaceOnRightList;
		for (auto visitedNode : visitedNodeSet)
		{
			if (!graphConfig->GetIsCompactBlueprintGraph() && !visitedNode->isToPlaceOnRight)
				continue;
			visitedNode->SetIsNotVisited();
			nodeToPlaceOnRightList.push_back(visitedNode);
		}
		for (auto nodeToPlaceOnRight : nodeToPlaceOnRightList)
			nodeToPlaceOnRight->PlaceOnRightInX(nodeToPlaceOnRightList);
	}

	void PlaceOnLeftInX(const std::vector<ExtendedNode*>& notFreeNodeAtLeftList)
	{
		if (GetIsVisited() || !contains(notFreeNodeAtLeftList, this))
			return;
		SetIsVisited();
		double scaleToAdd = graphConfig->GetIsMaterialOrSoundGraph() ? 1. : -1.;
		double posX = MAX_DOUBLE * scaleToAdd;

		// place nodes just at left of current node
		for (auto leftConnectionList : leftConnectionListArray)
		{
			for (auto leftConnection : *leftConnectionList)
			{
				if (!leftConnection.connectedToNode->GetIsVisited() && !contains(notFreeNodeAtLeftList, leftConnection.connectedToNode))
					continue;
				leftConnection.connectedToNode->PlaceOnLeftInX(notFreeNodeAtLeftList);
				double newPosX = leftConnection.connectedToNode->pos.x - leftConnection.deltaPos.x;
				if (posX < newPosX != graphConfig->GetIsMaterialOrSoundGraph())
					posX = newPosX;
				// update minPosX with only exec connection
				if (leftConnection.bExec)
				{
					// update minPosX for all comments not containing leftConnection.connectedNode
					for (auto containedByIExtendedComment : containedByIExtendedCommentSet)
					{
						if (contains(containedByIExtendedComment->getInsideNodeSet(), leftConnection.connectedToNode) ||
							contains(containedByIExtendedComment->getTempInsideNodeSet(), leftConnection.connectedToNode))
							continue;
						double sizeOffset = graphConfig->GetIsMaterialOrSoundGraph() ? size.x : 0;
						double newMinPosX = containedByIExtendedComment->updateMinPosX(newPosX + sizeOffset, graphConfig->GetIsMaterialOrSoundGraph()) - sizeOffset;
						if (posX < newMinPosX != graphConfig->GetIsMaterialOrSoundGraph())
							posX = newMinPosX;
					}
				}
			}
		}

		// get minPosX for all comments --> required for non exec connection
		for (auto containedByIExtendedComment : containedByIExtendedCommentSet)
		{
			double sizeOffset = graphConfig->GetIsMaterialOrSoundGraph() ? size.x : 0;
			double newMinPosX = containedByIExtendedComment->getMinPosX() - sizeOffset;
			if (posX < newMinPosX != graphConfig->GetIsMaterialOrSoundGraph())
				posX = newMinPosX;
		}

		if (posX != MAX_DOUBLE * scaleToAdd) // for safety --> should always be true
			SET_POS(PLACE_X_ON_LEFT, Vector2(posX, pos.y));
	}

	void PlaceOnRightInX(const std::vector<ExtendedNode*>& nodeAtLeftList)
	{
		if (GetIsVisited() || !contains(nodeAtLeftList, this))
			return;
		SetIsVisited();
		double scaleToAdd = graphConfig->GetIsMaterialOrSoundGraph() ? -1. : 1.;
		double posX = MAX_DOUBLE * scaleToAdd;

		// place nodes just at right of current node
		for (auto rightConnectionList : rightConnectionListArray)
		{
			for (auto rightConnection : *rightConnectionList)
			{
				if (!rightConnection.connectedToNode->GetIsVisited() && !contains(nodeAtLeftList, rightConnection.connectedToNode))
					continue;
				rightConnection.connectedToNode->PlaceOnRightInX(nodeAtLeftList);
				double newPosX = rightConnection.connectedToNode->pos.x - rightConnection.deltaPos.x;
				if (posX > newPosX != graphConfig->GetIsMaterialOrSoundGraph())
					posX = newPosX;
			}
		}
		
		if (posX != MAX_DOUBLE * scaleToAdd) // for safety --> should always be true
			SET_POS(PLACE_X_ON_RIGHT, Vector2(posX, pos.y));
	}

	// return true if current node is not free
	bool GetNotFreeNodeAtLeftList(std::vector<ExtendedNode*>& notFreeNodeOnLeftList, bool isOnLeftOfRerouteOneOutput, std::set<ExtendedNode*>& visitedNodeSet)
	{
		isToPlaceOnRight &= !isOnLeftOfRerouteOneOutput;
		if (GetIsVisited())
			return true;
		if (contains(visitedNodeSet, this))
			return contains(notFreeNodeOnLeftList, this);
		visitedNodeSet.insert(this);
		bool isNotFree = false;
		for (auto leftConnectionList : leftConnectionListArray)
			for (auto leftConnection : *leftConnectionList)
				if (leftConnection.connectedToNode->GetNotFreeNodeAtLeftList(notFreeNodeOnLeftList,
					isOnLeftOfRerouteOneOutput || this->isRerouteNodeOneOutput, visitedNodeSet))
					isNotFree = true;
		if (isNotFree)
			notFreeNodeOnLeftList.push_back(this);
		else
			isToPlaceOnRight = true;
		return isNotFree;
	}

	void SetRerouteLoopNodeVisitedInX()
	{
		if (GetIsVisited())
			return;
		SetIsVisited();
		rerouteLoopNodeBrother->SetIsVisited();
	}

// ======== ARRANGEMENT IN Y ========

	void PlaceInY()
	{
		PlaceInYParam placeInYParam;
		PlaceAllInY(pos.y, placeInYParam);
		while (!placeInYParam.nodeStack.empty())
		{
			auto lastNode = placeInYParam.nodeStack.back();
			placeInYParam.nodeStack.pop_back();

			lastNode->PlaceExecOnlyMostAtLeftInY(lastNode->pos.y, placeInYParam);
		}
	}

	static int getNewPlaceOrder()
	{
		static int64_t static_placeOrder = -1;
		return ++static_placeOrder;
	}

	void PlaceAllInY(double minPosY, PlaceInYParam& placeInYParam)
	{
		if (!PlaceNodeInY(minPosY, placeInYParam))
			return;
		if (graphConfig->GetIsCompactBlueprintGraph())
		{
			PlaceExecAtRightInY(placeInYParam);
			PlaceNonExecAtRightInY(placeInYParam);
			PlaceNonExecAtLeftInY(placeInYParam);
		}
		else
		{
			PlaceNonExecAtLeftInY(placeInYParam);
			PlaceExecAtRightInY(placeInYParam);
			PlaceNonExecAtRightInY(placeInYParam);
		}
	}

	// returns true if node newly placed
	bool PlaceNodeInY(double minPosY, PlaceInYParam& placeInYParam)
	{
		if (GetIsVisited())
			return false;

		SetIsVisited();
		placeInYParam.nodeStack.push_back(this);
		placeInYParam.placedNodeList.push_back(this);
		UpdatePlaceOrder();

		if (graphConfig->bCenterize)
		{
			updateRemainingCenterizeNodeToPlaceCount();
			if (remainingCenterizeNodeToPlaceCount == 0)
				centerizeBP(placeInYParam);
		}
		
		// move current node in order to not overlap with node already placed
		GetPosForNoOverlapNodeInY(minPosY, placeInYParam.placedNodeList, placeInYParam.placedCommentList);
		SET_POS(PLACE_Y_NO_NODE_OVERLAP, Vector2(pos.x, minPosY));

		// move current node in order to make connections not overlap with connections already placed
		std::set<Connection*> connectionSetToAppend = ComputeConnectionSetToAppend(placeInYParam.placedNodeList);
		PlaceForNoConnectionOverlapInY(placeInYParam.placedConnectionSet, connectionSetToAppend);
		placeInYParam.placedConnectionSet.insert(connectionSetToAppend.begin(), connectionSetToAppend.end());

		ApplyFullyPlacedCommentOverlap(placeInYParam);

		if (graphConfig->bCenterize)
		{
			for (auto forCenterizeToUpdateNode : forCenterizeToUpdateNodeList)
			{
				forCenterizeToUpdateNode->remainingCenterizeNodeToPlaceCount--;
				if (forCenterizeToUpdateNode->remainingCenterizeNodeToPlaceCount == 0)
				{
					forCenterizeToUpdateNode->centerizeBP(placeInYParam);
					forCenterizeToUpdateNode->UpdatePosForNoOverlapNodeInY(placeInYParam.placedNodeList, placeInYParam.placedCommentList);
					forCenterizeToUpdateNode->ApplyFullyPlacedCommentOverlap(placeInYParam, false);
				}
			}
		}

		return true;
	}

	void PlaceNonExecAtLeftInY(PlaceInYParam& placeInYParam)
	{
		if (!leftNonExecConnectionList.empty())
		{
			// get first left non exec position
			double newMinPosY = pos.y + leftNonExecConnectionList[0].deltaPos.y;
			leftNonExecConnectionList[0].connectedToNode->GetFirstAtDirectionMinPosY(newMinPosY, true, false, placeInYParam.placedNodeList, placeInYParam.placedCommentList);

			// place left non exec
			for (auto leftNonExecConnection : leftNonExecConnectionList)
			{
				newMinPosY = std::max(newMinPosY, pos.y + leftNonExecConnection.deltaPos.y);
				leftNonExecConnection.connectedToNode->PlaceAllInY(newMinPosY, placeInYParam);
			}
		}
	}

	void PlaceExecAtRightInY(PlaceInYParam& placeInYParam)
	{
		for (auto rightExecConnection : rightExecConnectionList)
		{
			if (rightExecConnection.connectedToNode->GetIsVisited() || rightExecConnection.connectedToNode->rerouteLoopNodeParent)
				continue;
			double minPosY = pos.y + rightExecConnection.deltaPos.y;
			rightExecConnection.connectedToNode->GetFirstAtDirectionMinPosY(minPosY, false, true, placeInYParam.placedNodeList,
				placeInYParam.placedCommentList);
			rightExecConnection.connectedToNode->refNode = this;
			rightExecConnection.connectedToNode->refExec = true;
			rightExecConnection.connectedToNode->PlaceAllInY(minPosY, placeInYParam);
		}
		
		if (rerouteLoopNodeChild)
			rerouteLoopNodeChild->PlaceRerouteLoopNodeInXY(placeInYParam);
	}

	void PlaceNonExecAtRightInY(PlaceInYParam& placeInYParam)
	{
		if (!rightNonExecConnectionList.empty())
		{
			// get first right non exec position
			double newMinPosY = pos.y + rightNonExecConnectionList[0].deltaPos.y;

			// place right non exec
			for (auto rightNonExecConnection : rightNonExecConnectionList)
			{
				if (rightNonExecConnection.connectedToNode->GetWillBePlacedAtRight())
					continue;
				newMinPosY = std::max(newMinPosY, pos.y + rightNonExecConnection.deltaPos.y);
				rightNonExecConnection.connectedToNode->refNode = this;
				rightNonExecConnection.connectedToNode->refExec = false;
				rightNonExecConnection.connectedToNode->PlaceAllInY(newMinPosY, placeInYParam);
			}
		}
	}

	void UpdatePlaceOrder()
	{
		placeOrder = getNewPlaceOrder();
		for (auto containedByIExtendedComment : containedByIExtendedCommentSet)
		{
			if (containedByIExtendedComment->placeOrder() == -1)
				containedByIExtendedComment->placeOrder() = getNewPlaceOrder();
		}
	}

	void ApplyFullyPlacedCommentOverlap(PlaceInYParam& placeInYParam, bool bIncreasePlacedNodeCount = true)
	{
		for (auto containedByIExtendedComment : containedByIExtendedCommentSet)
		{
			if (bIncreasePlacedNodeCount)
				containedByIExtendedComment->increasePlacedNodeCount();
			if (containedByIExtendedComment->isFullyPlaced())
			{
				containedByIExtendedComment->updateCommentBox(graphConfig->commentSpacing);
				placeInYParam.placedCommentList.push_back(containedByIExtendedComment);
				containedByIExtendedComment->ApplyFullyPlaced(graphConfig->spacing, graphConfig->commentSpacing, graphConfig->GetIsAI_Graph(),
					placeInYParam.placedNodeList, placeInYParam.placedCommentList);
			}
		}
	}

	void PlaceExecOnlyMostAtLeftInY(double minPosY, PlaceInYParam& placeInYParam)
	{
		if (leftExecConnectionList.empty())
		{
			GetFirstAtDirectionMinPosY(minPosY, false, true, placeInYParam.placedNodeList, placeInYParam.placedCommentList);
			PlaceAllInY(minPosY, placeInYParam);
		}
		else
		{
			minPosY = GetIsVisited() ? pos.y : minPosY;
			for (auto leftExecConnection : leftExecConnectionList)
				leftExecConnection.connectedToNode->PlaceExecOnlyMostAtLeftInY(minPosY + leftExecConnection.deltaPos.y, placeInYParam);
		}
	}

	void GetFirstAtDirectionMinPosY(double& newMinPosY, bool bLeft, bool bExec, const std::vector<ExtendedNode*>& placedNodeList,
		const std::vector<IExtendedComment*>& placedCommentList)
	{
		GetPosForNoOverlapNodeInY(newMinPosY, placedNodeList, placedCommentList);
		auto& connectionList = touchExtendedConnectionList(bLeft, bExec);
		int i = 0;
		while (i < connectionList.size() && (connectionList[i].bCompact || connectionList[i].connectedToNode->GetIsVisited()))
			i++;
		if (i == connectionList.size())
			return;
		newMinPosY += connectionList[i].deltaPos.y;
		connectionList[i].connectedToNode->GetFirstAtDirectionMinPosY(newMinPosY, bLeft, bExec, placedNodeList, placedCommentList);
		newMinPosY -= connectionList[i].deltaPos.y;
	}

	void GetPosForNoOverlapNodeInY(double& minPosY, const std::vector<ExtendedNode*>& placedNodeList, const std::vector<IExtendedComment*>& placedCommentList)
	{
		Box box = computeNodeBox();
		Vector2 halfSpacingY = Vector2(0., 0.5 * graphConfig->spacing.y);
		bool hasIntersection = true;
		while (hasIntersection)
		{
			hasIntersection = false;
			for (auto placedComment : placedCommentList)
			{
				if (contains(placedComment->getInsideNodeSet(), this) || getOrComputeCommentDiffCount(placedComment) != 1)
					continue;
				const Box nodeBox = extendBoxWithComment(box, placedComment, graphConfig->commentSpacing).offsetBox(Vector2(0., minPosY - pos.y));
				const Box placedCommentBox = placedComment->extendBoxWithComment(placedComment->getCommentBox(), this, graphConfig->commentSpacing)
					.extendBox(halfSpacingY).offsetBox(halfSpacingY);
				if (nodeBox.IntersectWith(placedCommentBox))
				{
					hasIntersection = true;
					minPosY += placedCommentBox.getMax().y - nodeBox.getMin().y + 1;
				}
			}
			for (auto placedNode : placedNodeList)
			{
				if (placedNode == this || getOrComputeCommentDiffCount(placedNode) != 0)
					continue;
				const Box nodeBox = extendBoxWithComment(box, placedNode, graphConfig->commentSpacing).offsetBox(Vector2(0., minPosY - pos.y));
				const Box placedNodeBox = placedNode->extendBoxWithComment(placedNode->computeNodeBox(), this, graphConfig->commentSpacing)
					.extendBox(halfSpacingY).offsetBox(halfSpacingY);
				if (nodeBox.IntersectWith(placedNodeBox))
				{
					hasIntersection = true;
					minPosY += placedNodeBox.getMax().y - nodeBox.getMin().y + 1;
				}
			}
		}
	}

	void PlaceForNoConnectionOverlapInY(const std::set<Connection*>& placedConnectionSet, const std::set<Connection*>& connectionSetToAppend)
	{
		bool bConnectionOverlapping = false;
		for (auto connection : connectionSetToAppend)
		{
			bool isConnectionAligned = false;
			Box connectionBox = connection->ComputeConnectionBox(isConnectionAligned);
			if (!isConnectionAligned)
				continue;
			for (auto placedConnection : placedConnectionSet)
			{
				if (*connection == *placedConnection || *connection == *placedConnection->GetOppositeConnection())
					continue;
				bool bPlacedConnectionAligned = false;
				Box placedConnectionBox = placedConnection->ComputeConnectionBox(bPlacedConnectionAligned);
				if (!bPlacedConnectionAligned)
					continue;
				bConnectionOverlapping = connectionBox.IntersectWith(placedConnectionBox);
				if (bConnectionOverlapping)
					break;
			}
			if (bConnectionOverlapping)
				break;
		}
		if (bConnectionOverlapping)
			SET_POS(PLACE_Y_NO_CONNECTION_OVERLAP, Vector2(pos.x, pos.y + SCALE_LINE_WIDTH * graphConfig->lineWidth));
	}

	void PlaceRerouteLoopNodeInXY(PlaceInYParam& placeInYParam)
	{
		if (GetIsVisited() || !rerouteLoopNodeParent->GetIsVisited() || !rerouteLoopNodeBrother->rerouteLoopNodeParent->GetIsVisited())
			return;
		if (!isRerouteLoopNodePlacedOnRight)
		{
			rerouteLoopNodeBrother->PlaceRerouteLoopNodeInXY(placeInYParam);
			return;
		}
		SetIsVisited();
		placeInYParam.placedNodeList.push_back(this);
		rerouteLoopNodeBrother->SetIsVisited();
		placeInYParam.placedNodeList.push_back(rerouteLoopNodeBrother);
		SET_POS(PLACE_Y_REROUTE_LOOP,
			rerouteLoopNodeParent->pos - (leftExecConnectionList.empty() ? leftNonExecConnectionList : leftExecConnectionList)[0].deltaPos);
		rerouteLoopNodeBrother->SET_POS(PLACE_Y_REROUTE_LOOP,
			rerouteLoopNodeBrother->rerouteLoopNodeParent->pos + (rightExecConnectionList.empty() ? rightNonExecConnectionList : rightExecConnectionList)[0].deltaPos);
		double minPosY = std::max(pos.y, rerouteLoopNodeBrother->pos.y);
		double oldMinPosY = minPosY - 1.;
		while (minPosY != oldMinPosY)
		{
			oldMinPosY = minPosY;
			// move current and brother node in order to not overlap with node already placed
			GetPosForNoOverlapNodeInY(minPosY, placeInYParam.placedNodeList, placeInYParam.placedCommentList);
			rerouteLoopNodeBrother->GetPosForNoOverlapNodeInY(minPosY, placeInYParam.placedNodeList, placeInYParam.placedCommentList);
 		}
		SET_POS(PLACE_Y_NO_NODE_OVERLAP, Vector2(pos.x, minPosY));
		rerouteLoopNodeBrother->SET_POS(PLACE_Y_NO_NODE_OVERLAP, Vector2(rerouteLoopNodeBrother->pos.x, minPosY));
		// move current node in order to make connections not overlap with connections already placed
		std::set<Connection*> connectionSetToAppend = ComputeConnectionSetToAppend(placeInYParam.placedNodeList);
		PlaceForNoConnectionOverlapInY(placeInYParam.placedConnectionSet, connectionSetToAppend);
		placeInYParam.placedConnectionSet.insert(connectionSetToAppend.begin(), connectionSetToAppend.end());
		if (rerouteLoopNodeBrother->pos.y < pos.y) // align brother if needed
			rerouteLoopNodeBrother->SET_POS(PLACE_Y_NO_CONNECTION_OVERLAP, Vector2(rerouteLoopNodeBrother->pos.x, pos.y));

		ApplyFullyPlacedCommentOverlap(placeInYParam);
		rerouteLoopNodeBrother->ApplyFullyPlacedCommentOverlap(placeInYParam);
	}

	void offsetAll(const Vector2& toAdd)
	{
		if (GetIsVisited())
			return;
		SetIsVisited();
		SET_POS(PLACE_OFFSET, pos + toAdd);
		for (auto allConnectionList : allConnectionListArray)
				for (auto& connection : *allConnectionList)
					connection.connectedToNode->offsetAll(toAdd);
	}

// ======== AI ARRANGEMENT METHODS ========

// ======== AI ARRANGEMENT IN X ========

	void AI_PlaceInX()
	{
		AI_PlaceNodeInX(pos.x);
	}

	// return vector containing current node and all most on right descendants 
	std::vector<ExtendedNode*> AI_PlaceNodeInX(double minPosX)
	{
		std::vector<ExtendedNode*> result = std::vector<ExtendedNode*>({this});
		if (GetIsVisited()) // this should never happen
			return result;
		SetIsVisited();
		SET_POS(AI_PLACE_X, Vector2(minPosX, pos.y));
		std::vector<ExtendedNode*> justPlacedNodeList;
		for (auto rightExecConnection : rightExecConnectionList)
		{
			if (justPlacedNodeList.empty())
				justPlacedNodeList = rightExecConnection.connectedToNode->AI_PlaceNodeInX(pos.x + rightExecConnection.deltaPos.x);
			else
			{
				double newMinPosX = justPlacedNodeList.back()->pos.x;
				for (auto justPlacedNode : justPlacedNodeList)
				{
					newMinPosX = std::max(newMinPosX,
						justPlacedNode->pos.x + justPlacedNode->size.x + graphConfig->spacing.x
						+ justPlacedNode->getOrComputeCommentDiffCount(rightExecConnection.connectedToNode) * graphConfig->commentSpacing.x);
				}
				justPlacedNodeList = rightExecConnection.connectedToNode->AI_PlaceNodeInX(newMinPosX);
			}
		}
		if (!rightExecConnectionList.empty())
		{
			// add all most on right descendants
			std::move(justPlacedNodeList.begin(), justPlacedNodeList.end(), std::back_inserter(result));
			if (graphConfig->bCenterize)
			{
				Vector2 toAdd = Vector2((rightExecConnectionList.back().connectedToNode->computeNodeBox().getMax().x -
					rightExecConnectionList.front().connectedToNode->computeNodeBox().getMin().x - size.x)  * -0.5, 0);
				SET_POS(AI_PLACE_OFFSET, pos - toAdd);
				std::set<ExtendedNode*> visitedNodeSet;
				AI_OffsetRight(toAdd, visitedNodeSet);
			}
		}
		return result;
	}

	void AI_OffsetRight(const Vector2& toAdd, std::set<ExtendedNode*>& visitedNodeSet)
	{
		if (contains(visitedNodeSet, this))
			return;
		visitedNodeSet.insert(this);
		SET_POS(AI_PLACE_OFFSET, pos + toAdd);
		for (auto rightExecConnection : rightExecConnectionList)
			rightExecConnection.connectedToNode->AI_OffsetRight(toAdd, visitedNodeSet);
	}

// ======== AI ARRANGEMENT IN Y ========

	void AI_PlaceInY()
	{
		PlaceInY();
	}

// ======== CENTER ARRANGEMENT ========

	void updateRemainingCenterizeNodeToPlaceCount()
	{
		if (rerouteLoopNodeParent || (!graphConfig->GetIsMaterialOrSoundGraph() && !leftExecConnectionList.empty()))
			return;
		remainingCenterizeNodeToPlaceCount = 0;
		for (auto& connection : graphConfig->GetIsMaterialOrSoundGraph() ? rightExecConnectionList : leftNonExecConnectionList)
		{
			if (!connection.connectedToNode->GetIsVisited() && (getOrComputeCommentDiffCount(connection.connectedToNode) == 0))
			{
				remainingCenterizeNodeToPlaceCount++;
				connection.connectedToNode->forCenterizeToUpdateNodeList.push_back(this);
			}
		}
	}

	// to call when remainingCenterizeNodeToPlaceCount == 0
	void centerizeBP(PlaceInYParam& placeInYParam)
	{
		double sumCoeff = 0.;
		Vector2 targetPos;
		for (auto& connection : graphConfig->GetIsMaterialOrSoundGraph() ? rightExecConnectionList : leftNonExecConnectionList)
		{
			if (getOrComputeCommentDiffCount(connection.connectedToNode) != 0)
				continue;
			sumCoeff++;
			targetPos += connection.connectedToNode->pos - connection.deltaPos;
		}
		if (sumCoeff != 0.)
		{
			targetPos = (targetPos / sumCoeff).maxWith(pos);
			SET_POS(PLACE_CENTERIZED, Vector2(pos.x, targetPos.y));
			for (auto& forCenterizeToUpdateNode : forCenterizeToUpdateNodeList)
			{
				if (forCenterizeToUpdateNode->remainingCenterizeNodeToPlaceCount == 0)
				{
					forCenterizeToUpdateNode->centerizeBP(placeInYParam);
					forCenterizeToUpdateNode->UpdatePosForNoOverlapNodeInY(placeInYParam.placedNodeList, placeInYParam.placedCommentList);
					forCenterizeToUpdateNode->ApplyFullyPlacedCommentOverlap(placeInYParam, false);
				}
			}
		}
	}
};

std::vector<NodeInstruction>* ExtendedNode::nodeInstructionList = nullptr;
CppGraphConfig* ExtendedNode::graphConfig = nullptr;
bool* ExtendedNode::bVisitedValue = nullptr;
