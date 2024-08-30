// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "EdGraph/EdGraphNode.h"
#include "EdGraphNode_Comment.h"
#include "SGraphPanel.h"

struct FGraphConfig;

struct CommentSet;

enum class EGraphTypeEnum : uint8;

class ANA_SelectionHelper
{
	ANA_SelectionHelper() = delete;

public:
	static void SelectConnectedGraph(SGraphPanel* graphPanel);

private:
	static void UpdateCommentMap(TSet<UEdGraphNode*>& selectedNodeSet, TSet<UEdGraphNode_Comment*>& selectedCommentSet,
	TMap<UEdGraphNode_Comment*, TSet<UEdGraphNode*>>& containedByCommentNodeSetMap, SGraphPanel* graphPanel);

	static bool SelectAllCommentsContainingOnlySelectedNodes(const TSet<UEdGraphNode*>& selectedNodeSet, TSet<UEdGraphNode_Comment*>& selectedCommentSet,
		const TMap<UEdGraphNode_Comment*, TSet<UEdGraphNode*>>& containedByCommentNodeSetMap);

	static TMap<UEdGraphNode*, CommentSet> GetContainingNodeCommentSetMap(const TSet<UEdGraphNode_Comment*>& selectedCommentSet,
		const TMap<UEdGraphNode_Comment*, TSet<UEdGraphNode*>>& containedByCommentNodeSetMap);

	// step 1
	static bool SelectAllNodesContainedBySelectedComments(TSet<UEdGraphNode*>& selectedNodeSet, const TSet<UEdGraphNode_Comment*>& selectedCommentSet,
		const TMap<UEdGraphNode_Comment*, TSet<UEdGraphNode*>>& containedByCommentNodeSetMap);
	// step 2
	static bool SelectAllNodesConnectedToSelectedNodesInSelectedComments(TSet<UEdGraphNode*>& selectedNodeSet, const TSet<CommentSet>& allowedCommentGroupSet,
		const TMap<UEdGraphNode*, CommentSet>& containingNodeCommentSetMap);
	static bool RecursiveSelectAllNodesConnectedToSelectedNodesInSelectedComments(UEdGraphNode* currentNode, TSet<UEdGraphNode*>& selectedNodeSet,
		const TSet<CommentSet>& allowedCommentGroupSet, const TMap<UEdGraphNode*, CommentSet>& containingNodeCommentSetMap);
	// step 3
	static bool SelectAllNodesConnectedToSelectedNodes(TSet<UEdGraphNode*>& selectedNodeSet);
	static bool RecuriveSelectAllNodesConnectedToSelectedNodes(UEdGraphNode* currentNode, TSet<UEdGraphNode*>& selectedNodeSet);
	// step 4
	static bool SelectAllCommentsContainingSelectedNodes(const TSet<UEdGraphNode*>& selectedNodeSet, TSet<UEdGraphNode_Comment*>& selectedCommentSet,
		const TMap<UEdGraphNode*, CommentSet>& containingNodeCommentSetMap);
};
