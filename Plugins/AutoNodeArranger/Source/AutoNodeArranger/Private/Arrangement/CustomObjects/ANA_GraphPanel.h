// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "../../Config/ANA_EditorConfig.h"
#include "../../Config/ANA_ProjectConfig.h"
#include "EdGraphNode_Comment.h"
#include "EdGraphSchema_K2_Actions.h"
#include "K2Node_Knot.h"
#include "SGraphPanel.h"

#include "ArrangerImp/_ExtendedObjects/ExtendedGraph.h"
#include "../../Helpers/ANA_RerouteHelper.h"

#define USE_EXE false

enum ESelectionType
{
	OnlySelected,
	OnlyComments,
	All,
	Connected
};

/**
 *
 */
class ANA_GraphPanel
{
	// ======== CONSTRUCTORS ========

	//
public:

	static ANA_GraphPanel instance;

	ANA_GraphPanel(SGraphPanel* _graphPanel, EGraphTypeEnum _graphType);

	// ======== VARIABLES ========

	//
public:

	// attributes set on construct
	SGraphPanel* graphPanel = nullptr;
	
	bool isArrangeNodeTransacting = false;
	float speed = 1.f;

#if USE_EXE
	bool isReadyToRead = false;
#endif

private:
	// attributes set on construct
	EGraphTypeEnum graphType;
	FNodePropertiesMap nodePropertiesMap;

	// attribute updated on auto arrange
	static UEdGraph* autoArrangeGraph;
	static TSet<UEdGraphNode*> autoArrangeSelectedNodeSet;

	// attributes to set on arrange
	FGraphConfig graphConfig;
	bool bCompact;
	bool bCenterize;
	ESelectionType selectionType;
	bool isAutoSizeCommentsModuleLoaded;
	TArray<int> selectedNodeIDArray;
	TArray<int> selectedCommentIDArray;

	TArray<UEdGraphNode_Comment*> commentIDToComment;
	TArray<UEdGraphNode*> nodeIDToNode;
	TArray<UEdGraphPin*> pinIDToPin;
	ExtendedGraph extendedGraph;

	TMap<UEdGraphPin*, int> pinToPinIDMap;

#if USE_EXE
	static const char* export_graph_filename;
	static const char* arranger_main_filename;
	static const char* import_instructions_filename;
#endif

	// ======== FUNCTIONS ========

	//
public:

	// Clean all notified nodes and end arrange node transaction
	void StopArrangeGraph();
	
	// If isAutoSizeCommentsModuleLoaded, destroy all old comment and set node selection
	void FinalizeArrangeGraph();

	static void UpdateAutoArrangeSelectedNodeSet();

	static void ClearAutoArrangeSelectedNodeSet();

	// Change targetPos to get an arranged graph and notify all nodes whose targetPos has changed
	void ComputeArrangeGraph(bool _bCompact, bool _bCenterize, const FGraphConfig& _graphConfig);

	int AddComment(UEdGraphNode_Comment* comment); // return commentID
	int AddNode(UEdGraphNode* node, ANA_RerouteHelper::IsRerouteFunc isRerouteFunc); // return nodeID
	void AddNodePins(UEdGraphNode* graphNode, CppNode& node);
	void AddSelectedComment(const TArray<UEdGraphNode_Comment*>& graphCommentList);
	void AddSelectedNode(const TArray<UEdGraphNode*>& graphNodeList, ANA_RerouteHelper::IsRerouteFunc isRerouteFunc);
	void AddAllComment(const TArray<UEdGraphNode_Comment*>& graphCommentList);
	void AddAllNode(const TArray<UEdGraphNode*>& graphNodeList, ANA_RerouteHelper::IsRerouteFunc isRerouteFunc);
	void AddNodeInsideAddedComment(const TArray<UEdGraphNode*>& graphNodeList, ANA_RerouteHelper::IsRerouteFunc isRerouteFunc);
	void AddNodeConnectedToAddedNode(ANA_RerouteHelper::IsRerouteFunc isRerouteFunc);
	void AddCommentContainingAddedNode(const TArray<UEdGraphNode_Comment*>& graphCommentList);
	void SetAllConnections();
	void SetAllNodeInsideComment();
	void SortComment();
	void ExecuteArrange();

	void BeginArrangeNodeTransaction();

	void EndArrangeNodeTransaction();

	inline int getMaxInstructionID() { return extendedGraph.instructionGlobalContainer.getMaxInstructionID(); }

	inline void allNodeInstructionListExecute(int instructionID, float alpha)
	{
		if (instructionID < extendedGraph.instructionGlobalContainer.allNodeInstructionList.size())
		{
			const NodeInstruction&  nodeInstruction = extendedGraph.instructionGlobalContainer.allNodeInstructionList[instructionID];
			executeNodeInstruction(nodeInstruction.nodeID, nodeInstruction.fromPos * (1- alpha) + nodeInstruction.toPos * alpha);
		}
		if (!isAutoSizeCommentsModuleLoaded && !extendedGraph.instructionGlobalContainer.commentInstructionList.empty())
		{
			for (const auto& commentInstruction : extendedGraph.instructionGlobalContainer.commentInstructionList)
			{
				if (commentInstruction.commentID >= extendedGraph.commentList.size())
					continue;
				Box currentCommentBox = extendedGraph.commentList[commentInstruction.commentID].computeCurrentCommentBox(extendedGraph.graphConfig.commentSpacing,
					extendedGraph.commentList, this);
				UpdatePosAndSizeForComment(commentInstruction.commentID, currentCommentBox.getMin(), currentCommentBox.getSize());
			}
		}
	}
	// Executed by ANA_Manager
	inline void mergedNodeInstructionListExecute(float alpha)
	{
		for (const NodeInstruction& mergedNodeInstructionL : extendedGraph.instructionGlobalContainer.mergedNodeInstructionList)
			executeNodeInstruction(mergedNodeInstructionL.nodeID, mergedNodeInstructionL.fromPos * (1 - alpha) + mergedNodeInstructionL.toPos * alpha);
		if (!isAutoSizeCommentsModuleLoaded && !extendedGraph.instructionGlobalContainer.commentInstructionList.empty())
		{
			for (const auto& commentInstruction : extendedGraph.instructionGlobalContainer.commentInstructionList)
			{
				if (commentInstruction.commentID >= extendedGraph.commentList.size())
					continue;
				Box currentCommentBox = extendedGraph.commentList[commentInstruction.commentID].computeCurrentCommentBox(extendedGraph.graphConfig.commentSpacing,
					extendedGraph.commentList, this);
				UpdatePosAndSizeForComment(commentInstruction.commentID, currentCommentBox.getMin(), currentCommentBox.getSize());
			}
			if (alpha == 1.f)
				commentInstructionListExecute();
		}
	}

	// Executed by nodeInstructionTree
	inline void executeNodeInstruction(int nodeID, Vector2 pos)
	{
		if (!nodeIDToNode.IsValidIndex(nodeID) || !IsValid(nodeIDToNode[nodeID]))
			return;
		nodeIDToNode[nodeID]->Modify();
		nodeIDToNode[nodeID]->NodePosX = pos.x;
		nodeIDToNode[nodeID]->NodePosY = pos.y;
	}

	// Executed by RerouteInstruction
	inline void executeRerouteInstruction(Vector2 pos)
	{
		UK2Node_Knot* rerouteNode = FEdGraphSchemaAction_K2NewNode::SpawnNode<UK2Node_Knot>(graphPanel->GetGraphObj(), FVector2D(pos.x, pos.y), EK2NewNodeFlags::None);
		nodeIDToNode.Add(rerouteNode);
		pinIDToPin.Add(rerouteNode->GetInputPin());
		pinIDToPin.Add(rerouteNode->GetOutputPin());
	}

	// Executed by LinkInstruction
	inline void executeLinkInstruction(int fromPinID, int toPinID, bool bBreak)
	{
		if (!pinIDToPin.IsValidIndex(fromPinID) || !pinIDToPin.IsValidIndex(toPinID))
			return;
		UEdGraphPin* fromPin = pinIDToPin[fromPinID];
		UEdGraphPin* toPin = pinIDToPin[toPinID];
		fromPin->Modify(); 
		toPin->Modify();
		if (bBreak)
			fromPin->BreakLinkTo(toPin);
		else
			fromPin->MakeLinkTo(toPin);
		UEdGraphNode* fromNode = fromPin->GetOwningNodeUnchecked();
		if (fromNode)
			fromNode->PinConnectionListChanged(fromPin);
		UEdGraphNode* toNode = toPin->GetOwningNodeUnchecked();
		if (toNode)
			toNode->PinConnectionListChanged(toPin);
	}

	// Executed by CommentInstruction
	inline void executeCommentInstructionForNode(int commentID, int nodeIDUnderComment)
	{
		if (!commentIDToComment.IsValidIndex(commentID) || !nodeIDToNode.IsValidIndex(nodeIDUnderComment)
			|| !IsValid(commentIDToComment[commentID]) || !IsValid(nodeIDToNode[nodeIDUnderComment]))
			return;
		commentIDToComment[commentID]->Modify();
		commentIDToComment[commentID]->AddNodeUnderComment(nodeIDToNode[nodeIDUnderComment]);
	}
	inline void executeCommentInstructionForComment(int commentID, int commentIDUnderComment)
	{
		if (!commentIDToComment.IsValidIndex(commentID) || !commentIDToComment.IsValidIndex(commentIDUnderComment)
			|| !IsValid(commentIDToComment[commentID]) || !IsValid(commentIDToComment[commentIDUnderComment]))
			return;
		commentIDToComment[commentID]->Modify();
		commentIDToComment[commentID]->AddNodeUnderComment(commentIDToComment[commentIDUnderComment]);
	}
	inline Box getCurrentNodeBox(int nodeID)
	{
		return Box::pos_size(Vector2(nodeIDToNode[nodeID]->NodePosX, nodeIDToNode[nodeID]->NodePosY), extendedGraph.nodeList[nodeID].size);
	}
	
	void ReadArrangedGraph();

private:
	// Update comment current pos and size in order to contain all inside nodes and comments
	void UpdatePosAndSizeForComment(int commentID, Vector2 pos, Vector2 size);

	inline void rerouteToCreateExecute()
	{
		graphPanel->GetGraphObj()->Modify();
		for (const RerouteInstruction& rerouteInstruction : extendedGraph.instructionGlobalContainer.rerouteInstructionList)
			executeRerouteInstruction(rerouteInstruction.pos);
	}

	inline void linkInstructionListExecute()
	{
		for (const LinkInstruction& linkInstruction : extendedGraph.instructionGlobalContainer.linkInstructionList)
			executeLinkInstruction(linkInstruction.fromPinID, linkInstruction.toPinID, linkInstruction.bBreak);
	}

	inline void commentInstructionListExecute()
	{
		for (const CommentInstruction& commentInstruction : extendedGraph.instructionGlobalContainer.commentInstructionList)
		{
			if (!commentIDToComment.IsValidIndex(commentInstruction.commentID) || !IsValid(commentIDToComment[commentInstruction.commentID]))
				continue;
			auto comment = commentIDToComment[commentInstruction.commentID];
			comment->Modify();
			comment->ClearNodesUnderComment();
			comment->CommentDepth = -1;
		}
		for (const CommentInstruction& commentInstruction : extendedGraph.instructionGlobalContainer.commentInstructionList)
		{
			for (auto nodeIDUnderComment : commentInstruction.nodeIDUnderCommentList)
				executeCommentInstructionForNode(commentInstruction.commentID, nodeIDUnderComment);
			for (auto commentIDUnderComment : commentInstruction.commentIDUnderCommentList)
				executeCommentInstructionForComment(commentInstruction.commentID, commentIDUnderComment);
		}
	}

};
