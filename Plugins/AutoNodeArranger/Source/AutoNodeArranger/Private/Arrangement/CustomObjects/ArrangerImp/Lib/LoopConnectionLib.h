// Copyright 2022 bstt, Inc. All Rights Reserved.

#include <vector>

#include "../_CustomObjects/Graph/GraphConfig.h"
#include "../_CustomObjects/Instruction/RerouteInstruction.h"
#include "../_CustomObjects/Instruction/LinkInstruction.h"

#include "../_ExtendedObjects/ExtendedNode.h"
#include "../_ExtendedObjects/ExtendedComment.h"

#include "../_ExtendedObjects/ExtendedCommon.h"

#include "../CDebug.h"

#define REROUTE_SIZE Vector2(40., 24.)
#define REROUTE_PIN_OFFSET Vector2(11.5, 7.5)

class LoopConnectionLib
{
// ======== ATTRIBUTES ========

public:
	const CppGraphConfig* graphConfig;
	std::vector<RerouteInstruction>* rerouteInstructionList = nullptr;
	std::vector<LinkInstruction>* linkInstructionList = nullptr;
	std::set<ExtendedNode*>* connectedToNodeSelectionNodeSet = nullptr;

	std::vector<ExtendedPin>* pinList = nullptr;
	std::vector<ExtendedNode>* nodeList = nullptr;

// ======== CONSTRUCTOR ========

	LoopConnectionLib(const CppGraphConfig* _graphConfig,
			std::vector<RerouteInstruction>* _rerouteInstructionList,
			std::vector<LinkInstruction>* _linkInstructionList,
			std::set<ExtendedNode*>* _connectedToNodeSelectionNodeSet,
			std::vector<ExtendedPin>* _pinList,
			std::vector<ExtendedNode>* _nodeList) :
		graphConfig(_graphConfig),
		rerouteInstructionList(_rerouteInstructionList),
		linkInstructionList(_linkInstructionList),
		connectedToNodeSelectionNodeSet(_connectedToNodeSelectionNodeSet),
		pinList(_pinList),
		nodeList(_nodeList) {}

// ======== METHODS ========

	void AutoGenerateRerouteLoopNode()
	{
		for (auto connection : GetConnectionToBreakList(false))
		{
			if (!connection->connectedFromNode->isRerouteNodeOneOutput)
				GenerateRerouteLoopNode(true, connection);
			if (!connection->connectedToNode->isRerouteNodeOneOutput)
				GenerateRerouteLoopNode(false, connection);
		}
	}

	void UpdateRerouteLoopNodeFamily()
	{
		for (auto connection : GetConnectionToBreakList(false))
		{
			bool isConnectedFromRerouteNodeOneOutput = connection->connectedFromNode->isRerouteNodeOneOutput;
			bool isConnectedToRerouteNodeOneOutput = connection->connectedToNode->isRerouteNodeOneOutput;
			if (isConnectedFromRerouteNodeOneOutput)
			{
				ExtendedNode::Connection* inputConnection = connection->connectedFromNode->leftExecConnectionList.empty() ?
					&connection->connectedFromNode->leftNonExecConnectionList[0] : &connection->connectedFromNode->leftExecConnectionList[0];
				connection->connectedFromNode->rerouteLoopNodeParent = inputConnection->connectedToNode;
				inputConnection->connectedToNode->rerouteLoopNodeChild = connection->connectedFromNode;
				connection->connectedFromNode->isRerouteLoopNodePlacedOnRight = true;

				inputConnection->deltaPos = connection->connectedFromNode->computeDeltaPosForRerotueNode(true);
				auto oppositeInputConnection = inputConnection->GetOppositeConnection();
				oppositeInputConnection->deltaPos = -inputConnection->deltaPos;
			}
			if (isConnectedToRerouteNodeOneOutput)
			{
				ExtendedNode::Connection* outputConnection = connection->connectedToNode->rightExecConnectionList.empty() ?
					&connection->connectedToNode->rightNonExecConnectionList[0] : &connection->connectedToNode->rightExecConnectionList[0];
				connection->connectedToNode->rerouteLoopNodeParent = outputConnection->connectedToNode;
				outputConnection->connectedToNode->rerouteLoopNodeChild = connection->connectedToNode;
				connection->connectedToNode->isRerouteLoopNodePlacedOnRight = false;

				outputConnection->deltaPos = connection->connectedToNode->computeDeltaPosForRerotueNode(false);
				auto oppositeOutputConnection = outputConnection->GetOppositeConnection();
				oppositeOutputConnection->deltaPos = -outputConnection->deltaPos;

				// outputConnection ALWAYS broken ==> copy deltaPos in connection
				connection->deltaPos = outputConnection->deltaPos;
				auto oppositeConnection = connection->GetOppositeConnection();
				oppositeConnection->deltaPos = -connection->deltaPos;
			}
			if (isConnectedFromRerouteNodeOneOutput && isConnectedToRerouteNodeOneOutput)
			{
				connection->connectedFromNode->rerouteLoopNodeBrother = connection->connectedToNode;
				connection->connectedToNode->rerouteLoopNodeBrother = connection->connectedFromNode;
			}
		}
	}

	void BreakAllLoops()
	{
		if (graphConfig->GetIsBlueprintGraph())
		{
			for (auto connection : GetConnectionToBreakList(true))
				connection->breakConnection(); // not added to linkInstructionList in order to not let the connection broken
		}
		else
		{
			for (auto connection : GetConnectionToBreakList(true))
				linkInstructionList->push_back(connection->breakConnection()); // add to linkInstructionList in order to let the connection broken
		}
	}

	std::vector<ExtendedNode::Connection*> GetConnectionToBreakList(bool bNextConnection)
	{
		std::vector<ExtendedNode::Connection*> connectionToBreakList;

		std::vector<ExtendedNode*> pathToCurrentNode;
		std::vector<ExtendedNode::Connection*> pathToCurrentConnection;

		for (auto connectedToNodeSelectionNode : *connectedToNodeSelectionNodeSet)
			RecursiveGetConnectionToBreakList(connectionToBreakList, connectedToNodeSelectionNode,
				bNextConnection, pathToCurrentNode, pathToCurrentConnection);
		return connectionToBreakList;
	}

	void RecursiveGetConnectionToBreakList(std::vector<ExtendedNode::Connection*>& connectionToBreakList, ExtendedNode* currentNode,
		bool bNextConnection, std::vector<ExtendedNode*>& pathToCurrentNode, std::vector<ExtendedNode::Connection*>& pathToCurrentConnection)
	{
		pathToCurrentNode.push_back(currentNode);
		for (auto connectionList : pathToCurrentNode.back()->rightConnectionListArray)
		{
			for (auto& connection : *connectionList)
			{
				if (contains(connectionToBreakList, &connection))
					continue;
				pathToCurrentConnection.push_back(&connection);
				if (contains(pathToCurrentNode, connection.connectedToNode))
				{
					int index = GetConnectionToBreakIndex(pathToCurrentConnection);
					if (bNextConnection && pathToCurrentConnection.size() > 2)
						index++;
					auto connectionToAdd = pathToCurrentConnection[index % pathToCurrentConnection.size()];
					if (!contains(connectionToBreakList, connectionToAdd))
						connectionToBreakList.push_back(connectionToAdd);
				}
				else
					RecursiveGetConnectionToBreakList(connectionToBreakList, connection.connectedToNode, bNextConnection, pathToCurrentNode, pathToCurrentConnection);
				pathToCurrentConnection.pop_back();
			}
		}
		pathToCurrentNode.pop_back();
	}

	int GetConnectionToBreakIndex(const std::vector<ExtendedNode::Connection*>& connectionList)
	{
		int result = 0;
		int resultOverride = -1; // override result
		float diffPosX_result = connectionList[0]->connectedToNode->pos.x - connectionList[0]->connectedFromNode->pos.x;
		float diffPosX_override = 0.f;
		for (int i = 0; i < connectionList.size(); i++)
		{
			if (connectionList[i]->connectedFromNode->isRerouteNodeOneOutput &&
				connectionList[i]->connectedToNode->isRerouteNodeOneOutput) // if connection between 2 reroute nodes with one output
				return i;
			float diffPosX_I = connectionList[i]->connectedToNode->pos.x - connectionList[i]->connectedFromNode->pos.x;
			if (connectionList[i]->connectedToNode->isRerouteNodeOneOutput) // if connection finish with a reroute node with one output
			{
				if (resultOverride == -1 || diffPosX_I < diffPosX_override) // get min diff X
				{
					resultOverride = i;
					diffPosX_override = diffPosX_I;
				}
			}
			if (diffPosX_I < diffPosX_result) // get min diff X
			{
				result = i;
				diffPosX_result = diffPosX_I;
			}
		}
		return resultOverride == -1 ? result : resultOverride;
	}

	void GenerateRerouteLoopNode(bool isLeft, ExtendedNode::Connection*& connection)
	{
		ExtendedPin* leftPin = connection->connectedFromPin;
		ExtendedPin* rightPin = connection->connectedToPin;
		ExtendedNode* leftNode = connection->connectedFromNode;
		ExtendedNode* rightNode = connection->connectedToNode;
		if (connection->bLeft) // right to left connection
		{
			leftPin = connection->connectedToPin;
			rightPin = connection->connectedFromPin;
			leftNode = connection->connectedToNode;
			rightNode = connection->connectedFromNode;
		}

		linkInstructionList->push_back(connection->breakConnection());

		Vector2 rerouteSize = REROUTE_SIZE;
		Vector2 reroutePinOffset = REROUTE_PIN_OFFSET;

		// Create Reroute Node
		std::string rerouteName = std::string("Reroute") + std::to_string(nodeList->size());
		Vector2 pos = isLeft ?
			Vector2(leftNode->pos.x + leftNode->size.x - rerouteSize.x, leftNode->pos.y + leftNode->size.y + graphConfig->spacing.y) :
			Vector2(rightNode->pos.x, rightNode->pos.y + rightNode->size.y + graphConfig->spacing.y);
		nodeList->emplace_back(nodeList->size(), rerouteName, pos, rerouteSize, true);
		ExtendedNode* rerouteNode = &nodeList->back();
		rerouteInstructionList->emplace_back(rerouteNode->pos);
		rerouteNode->isRerouteNodeOneOutput = true;
		rerouteNode->containedByIExtendedCommentSet = (isLeft ? leftNode : rightNode)->containedByIExtendedCommentSet;
		for (auto containedByIExtendedComment : rerouteNode->containedByIExtendedCommentSet)
			containedByIExtendedComment->addNodeInside(rerouteNode);

		// Create Pin
		pinList->emplace_back(pinList->size(), rerouteName + "_LeftPin", reroutePinOffset);
		ExtendedPin* inputPin = &pinList->back();
		pinList->emplace_back(pinList->size(), rerouteName + "_RightPin", reroutePinOffset);
		ExtendedPin* outputPin = &pinList->back();

		// Make connections
		linkInstructionList->push_back(makeLinkTo(true, connection->bExec, false, inputPin, leftPin, rerouteNode, leftNode, Vector2()));
		// leftConnection must be left to right
		ExtendedNode::Connection* leftConnection = &leftNode->touchExtendedConnectionList(false, connection->bExec).back();
		linkInstructionList->push_back(makeLinkTo(false, connection->bExec, false, outputPin, rightPin, rerouteNode, rightNode, Vector2()));
		// rightConnection must be left to right
		ExtendedNode::Connection* rightConnection = &rerouteNode->touchExtendedConnectionList(false, connection->bExec).back();

		connection = isLeft ? rightConnection : leftConnection;
	}

	// this function also update link on connectedToNode
	static LinkInstruction makeLinkTo(bool bLeft, bool bExec, bool bCompact, ExtendedPin* connectedFromPin, ExtendedPin* connectedToPin,
		ExtendedNode* connectedFromNode, ExtendedNode* connectedToNode, Vector2 deltaPos)
	{
		connectedFromPin->connectedToPinIDList.push_back(connectedToPin->ID);
		connectedToPin->connectedToPinIDList.push_back(connectedFromPin->ID);

		connectedFromNode->touchExtendedConnectionList(bLeft, bExec).emplace_back(bLeft, bExec, bCompact, connectedFromPin, connectedToPin,
			connectedFromNode, connectedToNode, deltaPos);
		connectedToNode->touchExtendedConnectionList(!bLeft, bExec).emplace_back(!bLeft, bExec, bCompact, connectedToPin, connectedFromPin,
			connectedToNode, connectedFromNode, deltaPos);

		connectedFromNode->updatePinIDListArray();
		connectedFromNode->updateConnectionListArray();
		connectedToNode->updatePinIDListArray();
		connectedToNode->updateConnectionListArray();

		return LinkInstruction(connectedFromPin->ID, connectedToPin->ID, false);
	}
};