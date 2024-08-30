// Copyright 2022 bstt, Inc. All Rights Reserved.

#include "ANA_SelectionHelper.h"
#include "ANA_NodeSizeHelper.h"
#include "../Config/ANA_ProjectConfig.h"
#include "../Config/ANA_EditorConfig.h"

struct CommentSet
{
	TSet<UEdGraphNode_Comment*> content;

	friend bool operator==(const CommentSet& lhs, const CommentSet& rhs)
	{
		return lhs.content.Union(rhs.content).Num() == lhs.content.Num() && lhs.content.Num() == rhs.content.Num();
	}

	friend uint32 GetTypeHash(const CommentSet& commentSet)
	{
		auto result = GetTypeHash(commentSet.content.Num());
		for (auto comment : commentSet.content)
			result = HashCombine(result, GetTypeHash(comment));
		return result;
	}
};

void ANA_SelectionHelper::SelectConnectedGraph(SGraphPanel* graphPanel)
{
	// Get all comments of the graph
	TSet<UEdGraphNode*> selectedNodeSet;
	TSet<UEdGraphNode_Comment*> selectedCommentSet;
	TMap<UEdGraphNode_Comment*, TSet<UEdGraphNode*>> containedByCommentNodeSetMap;
	UpdateCommentMap(selectedNodeSet, selectedCommentSet, containedByCommentNodeSetMap, graphPanel);

	auto editorConfig = UANA_EditorConfig::Get();

	SelectAllCommentsContainingOnlySelectedNodes(selectedNodeSet, selectedCommentSet, containedByCommentNodeSetMap);
	bool bNewSelection = true;
	while (bNewSelection)
	{
		bNewSelection = false;
		TMap<UEdGraphNode*, CommentSet> containingNodeCommentSetMap = GetContainingNodeCommentSetMap(selectedCommentSet, containedByCommentNodeSetMap);
		TSet<CommentSet> allowedCommentGroupSet;
		for (auto selectedNode : selectedNodeSet)
			allowedCommentGroupSet.Add(containingNodeCommentSetMap.FindRef(selectedNode));
		// step 1
		if (SelectAllNodesContainedBySelectedComments(selectedNodeSet, selectedCommentSet, containedByCommentNodeSetMap))
		{
			bNewSelection = true;
			if (editorConfig->bProgressiveSelection)
				break;
		}
		// step 2
		if (SelectAllNodesConnectedToSelectedNodesInSelectedComments(selectedNodeSet, allowedCommentGroupSet, containingNodeCommentSetMap))
		{
			bNewSelection = true;
			if (editorConfig->bProgressiveSelection)
				break;
			if (SelectAllCommentsContainingOnlySelectedNodes(selectedNodeSet, selectedCommentSet, containedByCommentNodeSetMap))
			{
				containingNodeCommentSetMap = GetContainingNodeCommentSetMap(selectedCommentSet, containedByCommentNodeSetMap);
				allowedCommentGroupSet.Reset();
				for (auto selectedNode : selectedNodeSet)
					allowedCommentGroupSet.Add(containingNodeCommentSetMap.FindRef(selectedNode));
			}
		}
		// step 3
		if (SelectAllNodesConnectedToSelectedNodes(selectedNodeSet))
		{
			bNewSelection = true;
			if (editorConfig->bProgressiveSelection)
				break;
			if (SelectAllCommentsContainingOnlySelectedNodes(selectedNodeSet, selectedCommentSet, containedByCommentNodeSetMap))
			{
				containingNodeCommentSetMap = GetContainingNodeCommentSetMap(selectedCommentSet, containedByCommentNodeSetMap);
				allowedCommentGroupSet.Reset();
				for (auto selectedNode : selectedNodeSet)
					allowedCommentGroupSet.Add(containingNodeCommentSetMap.FindRef(selectedNode));
			}
		}
		// step 4
		if (SelectAllCommentsContainingSelectedNodes(selectedNodeSet, selectedCommentSet, containingNodeCommentSetMap))
		{
			bNewSelection = true;
			if (editorConfig->bProgressiveSelection)
				break;
		}
	}
	SelectAllCommentsContainingOnlySelectedNodes(selectedNodeSet, selectedCommentSet, containedByCommentNodeSetMap);

	graphPanel->SelectionManager.ClearSelectionSet();
	for (auto selectedNode : selectedNodeSet)
		graphPanel->SelectionManager.SetNodeSelection(selectedNode, true);
	for (auto selectedComment : selectedCommentSet)
		graphPanel->SelectionManager.SetNodeSelection(selectedComment, true);
}

void ANA_SelectionHelper::UpdateCommentMap(TSet<UEdGraphNode*>& selectedNodeSet, TSet<UEdGraphNode_Comment*>& selectedCommentSet,
	TMap<UEdGraphNode_Comment*, TSet<UEdGraphNode*>>& containedByCommentNodeSetMap, SGraphPanel* graphPanel)
{
	FNodePropertiesMap nodePropertiesMap = UANA_ProjectConfig::GetNodePropertiesMap(graphPanel);
	for (auto node : graphPanel->GetGraphObj()->Nodes)
	{
		UEdGraphNode_Comment* nodeComment = Cast<UEdGraphNode_Comment>(node);
		if (!nodeComment)
		{
			if (graphPanel->SelectionManager.IsNodeSelected(node))
				selectedNodeSet.Add(node);
			continue;
		}
		if (graphPanel->SelectionManager.IsNodeSelected(node))
			selectedCommentSet.Add(nodeComment);

		FVector2D commentPos = FVector2D(nodeComment->NodePosX, nodeComment->NodePosY);
		FVector2D commentSize = GET_NODE_SIZE(nodeComment);

		FSlateRect commentRect = FSlateRect::FromPointAndExtent(commentPos, commentSize);
		TSet<UEdGraphNode*> insideNodeSet;
		for (auto node2 : graphPanel->GetGraphObj()->Nodes)
		{
			if (node2->GetClass()->IsChildOf<UEdGraphNode_Comment>())
				continue;
			FVector2D nodePos = FVector2D(node2->NodePosX, node2->NodePosY);
			FVector2D nodeSize = GET_NODE_SIZE(node2);
			FSlateRect nodeRect = FSlateRect::FromPointAndExtent(nodePos, nodeSize);
			if (FSlateRect::DoRectanglesIntersect(commentRect, nodeRect))
				insideNodeSet.Add(node2);
		}
		if (insideNodeSet.Num() > 0)
			containedByCommentNodeSetMap.Add(nodeComment, insideNodeSet);
	}
}

bool ANA_SelectionHelper::SelectAllCommentsContainingOnlySelectedNodes(const TSet<UEdGraphNode*>& selectedNodeSet, TSet<UEdGraphNode_Comment*>& selectedCommentSet,
	const TMap<UEdGraphNode_Comment*, TSet<UEdGraphNode*>>& containedByCommentNodeSetMap)
{
	bool result = false;
	for (auto containedByCommentNodeSet : containedByCommentNodeSetMap)
	{
		if (selectedCommentSet.Contains(containedByCommentNodeSet.Key))
			continue;
		bool bAllContainedNodeSelected = containedByCommentNodeSet.Value.Num() > 0;
		for (auto containedByCommentNode : containedByCommentNodeSet.Value)
		{
			if (!selectedNodeSet.Contains(containedByCommentNode))
				bAllContainedNodeSelected = false;
		}
		if (bAllContainedNodeSelected)
		{
			selectedCommentSet.Add(containedByCommentNodeSet.Key);
			result = true;
		}	
	}
	return result;
}

TMap<UEdGraphNode*, CommentSet> ANA_SelectionHelper::GetContainingNodeCommentSetMap(const TSet<UEdGraphNode_Comment*>& selectedCommentSet,
	const TMap<UEdGraphNode_Comment*, TSet<UEdGraphNode*>>& containedByCommentNodeSetMap)
{
	TMap<UEdGraphNode*, CommentSet> containingNodeCommentSetMap;
	for (auto containedByCommentNodeSet : containedByCommentNodeSetMap)
	{
		if (selectedCommentSet.Contains(containedByCommentNodeSet.Key)) // filter comment if comment is selected or all nodes inside comment are selected
			continue;
		for (auto containedByCommentNode : containedByCommentNodeSet.Value)
			containingNodeCommentSetMap.FindOrAdd(containedByCommentNode).content.Add(containedByCommentNodeSet.Key);
	}
	return containingNodeCommentSetMap;
}

// step 1
bool  ANA_SelectionHelper::SelectAllNodesContainedBySelectedComments(TSet<UEdGraphNode*>& selectedNodeSet, const TSet<UEdGraphNode_Comment*>& selectedCommentSet,
	const TMap<UEdGraphNode_Comment*, TSet<UEdGraphNode*>>& containedByCommentNodeSetMap)
{
	bool result = false;
	for (auto selectedComment : selectedCommentSet)
	{
		for (auto containedByCommentNode : containedByCommentNodeSetMap.FindRef(selectedComment))
		{
			bool isAlreadyPresentInSet = false;
			selectedNodeSet.Add(containedByCommentNode, &isAlreadyPresentInSet);
			result |= !isAlreadyPresentInSet;
		}
	}
	return result;
}

// step 2
bool ANA_SelectionHelper::SelectAllNodesConnectedToSelectedNodesInSelectedComments(TSet<UEdGraphNode*>& selectedNodeSet, const TSet<CommentSet>& allowedCommentGroupSet,
	const TMap<UEdGraphNode*, CommentSet>& containingNodeCommentSetMap)
{
	bool result = false;
	auto copySelectedNodeSet = selectedNodeSet;
	for (auto selectedNode : copySelectedNodeSet)
		for (UEdGraphPin* pin : selectedNode->Pins)
			for (UEdGraphPin* otherPin : pin->LinkedTo)				
				result = RecursiveSelectAllNodesConnectedToSelectedNodesInSelectedComments(otherPin->GetOwningNodeUnchecked(), selectedNodeSet, allowedCommentGroupSet,
					containingNodeCommentSetMap) || result;
	return result;
}

bool ANA_SelectionHelper::RecursiveSelectAllNodesConnectedToSelectedNodesInSelectedComments(UEdGraphNode* currentNode, TSet<UEdGraphNode*>& selectedNodeSet,
	const TSet<CommentSet>& allowedCommentGroupSet, const TMap<UEdGraphNode*, CommentSet>& containingNodeCommentSetMap)
{
	if (selectedNodeSet.Contains(currentNode))
		return false;
	const auto& containingCurrentNodeCommentSet = containingNodeCommentSetMap.FindRef(currentNode);
	if (!allowedCommentGroupSet.Contains(containingCurrentNodeCommentSet))
		return false;
	selectedNodeSet.Add(currentNode);
	for (UEdGraphPin* pin : currentNode->Pins)
		for (UEdGraphPin* otherPin : pin->LinkedTo)
			RecursiveSelectAllNodesConnectedToSelectedNodesInSelectedComments(otherPin->GetOwningNodeUnchecked(), selectedNodeSet, allowedCommentGroupSet, containingNodeCommentSetMap);
	return true;
}

// step 3
bool ANA_SelectionHelper::SelectAllNodesConnectedToSelectedNodes(TSet<UEdGraphNode*>& selectedNodeSet)
{
	bool result = false;
	auto copySelectedNodeSet = selectedNodeSet;
	for (auto selectedNode : copySelectedNodeSet)
		for (UEdGraphPin* pin : selectedNode->Pins)
			for (UEdGraphPin* otherPin : pin->LinkedTo)
				result = RecuriveSelectAllNodesConnectedToSelectedNodes(otherPin->GetOwningNodeUnchecked(), selectedNodeSet) || result;
	return result;
}

bool ANA_SelectionHelper::RecuriveSelectAllNodesConnectedToSelectedNodes(UEdGraphNode* currentNode, TSet<UEdGraphNode*>& selectedNodeSet)
{
	if (!currentNode || selectedNodeSet.Contains(currentNode))
		return false;
	selectedNodeSet.Add(currentNode);
	for (UEdGraphPin* pin : currentNode->Pins)
		for (UEdGraphPin* otherPin : pin->LinkedTo)
			RecuriveSelectAllNodesConnectedToSelectedNodes(otherPin->GetOwningNodeUnchecked(), selectedNodeSet);
	return true;
}

//step 4
bool ANA_SelectionHelper::SelectAllCommentsContainingSelectedNodes(const TSet<UEdGraphNode*>& selectedNodeSet, TSet<UEdGraphNode_Comment*>& selectedCommentSet,
	const TMap<UEdGraphNode*, CommentSet>& containingNodeCommentSetMap)
{
	bool result = false;
	for (auto selectedNode : selectedNodeSet)
	{
		for (auto containingNodeComment : containingNodeCommentSetMap.FindRef(selectedNode).content)
		{
			bool isAlreadyPresentInSet = false;
			selectedCommentSet.Add(containingNodeComment, &isAlreadyPresentInSet);
			result |= !isAlreadyPresentInSet;
		}
	}
	return result;
}

