// Copyright 2022 bstt, Inc. All Rights Reserved.

#include "ANA_GraphPanel.h"
#include "../../Helpers/ANA_NodeSizeHelper.h"
#include "SGraphNodeComment.h"
#include "Widgets/SWidget.h"
#include "../../Commands/ANA_ReleaseNotifier.h"

#include <fstream>

#if USE_EXE
#include "Runtime/Core/Public/Misc/InteractiveProcess.h"
#include "../../Debug/Debug.h"

const char* ANA_GraphPanel::export_graph_filename = __FILE__ "/../ArrangerImp/graph.txt";
const char* ANA_GraphPanel::arranger_main_filename = __FILE__ "/../ArrangerImp/ArrangerMain.exe";
const char* ANA_GraphPanel::import_instructions_filename = __FILE__ "/../ArrangerImp/instructions.txt";
#endif

ANA_GraphPanel ANA_GraphPanel::instance(nullptr, EGraphTypeEnum::Blueprint);
TSet<UEdGraphNode*> ANA_GraphPanel::autoArrangeSelectedNodeSet;
UEdGraph* ANA_GraphPanel::autoArrangeGraph = nullptr;

ANA_GraphPanel::ANA_GraphPanel(SGraphPanel* _graphPanel, EGraphTypeEnum _graphType)
{
	graphPanel = _graphPanel;
	graphType = _graphType;
	if (graphPanel)
		nodePropertiesMap = UANA_ProjectConfig::GetNodePropertiesMap(graphPanel);
}

void ANA_GraphPanel::StopArrangeGraph()
{
	EndArrangeNodeTransaction();
}

void ANA_GraphPanel::FinalizeArrangeGraph()
{
	if (isAutoSizeCommentsModuleLoaded)
		return;
	graphPanel->SelectionManager.ClearSelectionSet();
	for (auto selectedNodeID : selectedNodeIDArray)
		if (nodeIDToNode.IsValidIndex(selectedNodeID) && IsValid(nodeIDToNode[selectedNodeID]))
			graphPanel->SelectionManager.SetNodeSelection(nodeIDToNode[selectedNodeID], true);
	for (auto selectedCommentID : selectedCommentIDArray)
		if (commentIDToComment.IsValidIndex(selectedCommentID) && IsValid(commentIDToComment[selectedCommentID]))
			graphPanel->SelectionManager.SetNodeSelection(commentIDToComment[selectedCommentID], true);
}

void ANA_GraphPanel::UpdateAutoArrangeSelectedNodeSet()
{
	if (ANA_GraphPanel::instance.graphPanel->SelectionManager.SelectedNodes.Num() == 0)
		return;
	UObject* obj = (*ANA_GraphPanel::instance.graphPanel->SelectionManager.SelectedNodes.begin());
	UEdGraph* graph = Cast<UEdGraphNode>(obj)->GetGraph();
	if (graph != ANA_GraphPanel::instance.autoArrangeGraph)
	{
		ANA_GraphPanel::instance.autoArrangeGraph = graph;
		ANA_GraphPanel::instance.autoArrangeSelectedNodeSet.Reset();
	}
	for (auto selectedNode : ANA_GraphPanel::instance.graphPanel->SelectionManager.SelectedNodes)
		if (IsValid(selectedNode))
			ANA_GraphPanel::instance.autoArrangeSelectedNodeSet.Add(Cast<UEdGraphNode>(selectedNode));
}

void ANA_GraphPanel::ClearAutoArrangeSelectedNodeSet()
{
	ANA_GraphPanel::instance.autoArrangeSelectedNodeSet.Reset();
}

void ANA_GraphPanel::ComputeArrangeGraph(bool _bCompact, bool _bCenterize, const FGraphConfig& _graphConfig)
{
	graphConfig = _graphConfig;
	bCompact = _bCompact;
	bCenterize = _bCenterize;
	isAutoSizeCommentsModuleLoaded = FModuleManager::Get().IsModuleLoaded("AutoSizeComments");

	if (!IsValid(graphPanel->GetGraphObj()))
		return;

	EndArrangeNodeTransaction();

	// Convert config

	auto editorConfig = UANA_EditorConfig::Get();

	// Get Selection Type

	if (graphPanel->SelectionManager.SelectedNodes.Num() == 0 && autoArrangeSelectedNodeSet.Num() == 0)
		selectionType = ESelectionType::All;
	else if (editorConfig->arrangeSelectionType == EArrangeSelectionType::AlwaysSelected ||
		(editorConfig->arrangeSelectionType == EArrangeSelectionType::OneForAll
			&& (graphPanel->SelectionManager.SelectedNodes.Num() > 1 || autoArrangeSelectedNodeSet.Num() != 0
				|| (graphPanel->SelectionManager.SelectedNodes.Num() == 1 && (*graphPanel->SelectionManager.SelectedNodes.begin())->IsA<UEdGraphNode_Comment>()))))
	{
		selectionType = ESelectionType::OnlyComments;
		// check if only comments are selected
		for (auto selectedNode : graphPanel->SelectionManager.SelectedNodes)
		{
			if (IsValid(selectedNode) && !selectedNode->IsA<UEdGraphNode_Comment>())
			{
				selectionType = ESelectionType::OnlySelected;
				break;
			}
		}
	}
	else
		selectionType = ESelectionType::Connected;

	bool bSelectedNodeOrComments;
	switch (selectionType)
	{
	case OnlySelected:
	case OnlyComments:
		bSelectedNodeOrComments = true;
		break;
	case All:
		bSelectedNodeOrComments = false;
		break;
	case Connected:
		bSelectedNodeOrComments = false;
		break;
	default:
		bSelectedNodeOrComments = false;
		break;
	}

	Vector2 spacing = Vector2(bCompact ? graphConfig.CompactSpacing : graphConfig.Spacing);
	extendedGraph.graphConfig = CppGraphConfig(graphType == EGraphTypeEnum::Blueprint ? UANA_EditorConfig::Get()->ExecSpacingX : spacing.x,
		spacing,
		Vector2(graphConfig.CommentSpacing),
		editorConfig->bAutoGenerateReroute, !bSelectedNodeOrComments && editorConfig->bGroupAllConnectedGraph, editorConfig->LineWidth,
		bCompact, bCenterize,
		static_cast<CppGraphConfig::GraphType>(graphType));

	// Split nodes and comments
	TArray<UEdGraphNode*> graphNodeList;
	TArray<UEdGraphNode_Comment*> graphCommentList;
	for (auto node : graphPanel->GetGraphObj()->Nodes)
	{
		if (auto comment = Cast<UEdGraphNode_Comment>(node))
			graphCommentList.Add(comment);
		else
			graphNodeList.Add(node);
	}
	ANA_RerouteHelper::IsRerouteFunc isRerouteFunc = ANA_RerouteHelper::GetIsRerouteFunc(graphType == EGraphTypeEnum::Material);

	AddSelectedComment(graphCommentList);
	AddSelectedNode(graphNodeList, isRerouteFunc); // no node selected in case of bOnlyComments
	
	switch (selectionType)
	{
	case OnlySelected:
		// nothing to do here
		break;
	case OnlyComments:
		AddNodeInsideAddedComment(graphNodeList, isRerouteFunc);
		break;
	case All:
		AddAllComment(graphCommentList);
		AddAllNode(graphNodeList, isRerouteFunc);
		break;
	case Connected:
		AddNodeInsideAddedComment(graphNodeList, isRerouteFunc);
		AddNodeConnectedToAddedNode(isRerouteFunc);
		break;
	default:
		break;
	}

	SetAllConnections();
	AddCommentContainingAddedNode(graphCommentList);
	AddNodeInsideAddedComment(graphNodeList, isRerouteFunc);
	SetAllNodeInsideComment();
	SortComment();
	ExecuteArrange();

	UpdateAutoArrangeSelectedNodeSet(); // Keep all selected node in autoArrangeSelectedNodeSet
}

int ANA_GraphPanel::AddComment(UEdGraphNode_Comment* comment)
{
	std::string name = std::string(TCHAR_TO_UTF8(*comment->GetNodeTitle(ENodeTitleType::FullTitle).ToString()));
	int commentId = commentIDToComment.Add(comment);
	extendedGraph.commentList.emplace_back(commentId, name, Vector2(comment->NodePosX, comment->NodePosY), Vector2(GET_NODE_SIZE(comment)));
	extendedGraph.commentList.back().updateCommentBox(graphConfig.CommentSpacing);
	return commentId;
}

int ANA_GraphPanel::AddNode(UEdGraphNode* node, ANA_RerouteHelper::IsRerouteFunc isRerouteFunc)
{
	std::string name = std::string(TCHAR_TO_UTF8(*node->GetNodeTitle(ENodeTitleType::FullTitle).ToString()));
	int nodeId = nodeIDToNode.Add(node);
	extendedGraph.nodeList.emplace_back(nodeId, name, Vector2(node->NodePosX, node->NodePosY), Vector2(GET_NODE_SIZE(node)), isRerouteFunc(node));
	AddNodePins(node, extendedGraph.nodeList.back());
	return nodeId;
}

void ANA_GraphPanel::AddNodePins(UEdGraphNode* graphNode, CppNode& node)
{
	bool isGraphMaterialOrSound = (graphType == EGraphTypeEnum::Material || graphType == EGraphTypeEnum::Sound);

	for (auto pin : graphNode->Pins)
	{
		node.touchPinIDList(isGraphMaterialOrSound != (pin->Direction == EGPD_Input),
			graphType != EGraphTypeEnum::Blueprint || pin->PinType.PinCategory == TEXT("exec") || pin->PinType.PinCategory == TEXT("delegate")).push_back(pinIDToPin.Num());
		int pinId = pinIDToPin.Add(pin);
		extendedGraph.pinList.emplace_back(pinId, std::string(TCHAR_TO_UTF8(*pin->PinName.ToString())), Vector2(GET_PIN_OFFSET(graphNode, pin)));
		pinToPinIDMap.Add(pin, pinId);
	}
}

void ANA_GraphPanel::AddSelectedComment(const TArray<UEdGraphNode_Comment*>& graphCommentList)
{
	for (auto graphComment : graphCommentList)
	{
		if (commentIDToComment.Contains(graphComment))
			continue;
		if (graphPanel->SelectionManager.IsNodeSelected(graphComment) || autoArrangeSelectedNodeSet.Contains(graphComment))
		{
			int commentId = AddComment(graphComment);
			selectedCommentIDArray.Add(commentId);
			extendedGraph.selectedCommentIDList.push_back(commentId);
		}
	}
}

void ANA_GraphPanel::AddSelectedNode(const TArray<UEdGraphNode*>& graphNodeList, ANA_RerouteHelper::IsRerouteFunc isRerouteFunc)
{
	for (auto graphNode : graphNodeList)
	{
		if (nodeIDToNode.Contains(graphNode))
			continue;
		if (graphPanel->SelectionManager.IsNodeSelected(graphNode) || autoArrangeSelectedNodeSet.Contains(graphNode))
		{
			int nodeId = AddNode(graphNode, isRerouteFunc);
			selectedNodeIDArray.Add(nodeId);
			extendedGraph.selectedNodeIDList.push_back(nodeId);
		}
	}
}

void ANA_GraphPanel::AddAllComment(const TArray<UEdGraphNode_Comment*>& graphCommentList)
{
	for (auto graphComment : graphCommentList)
	{
		if (commentIDToComment.Contains(graphComment))
			continue;
		AddComment(graphComment);
	}
}

void ANA_GraphPanel::AddAllNode(const TArray<UEdGraphNode*>& graphNodeList, ANA_RerouteHelper::IsRerouteFunc isRerouteFunc)
{
	for (auto graphNode : graphNodeList)
	{
		if (nodeIDToNode.Contains(graphNode))
			continue;
		int nodeId = AddNode(graphNode, isRerouteFunc);
		extendedGraph.selectedNodeIDList.push_back(nodeId);
	}
}

void ANA_GraphPanel::AddNodeInsideAddedComment(const TArray<UEdGraphNode*>& graphNodeList, ANA_RerouteHelper::IsRerouteFunc isRerouteFunc)
{
	for (auto& comment : extendedGraph.commentList)
	{
		for (auto graphNode : graphNodeList)
		{
			if (nodeIDToNode.Contains(graphNode))
				continue;
			Vector2 nodePos = Vector2(graphNode->NodePosX, graphNode->NodePosY);
			Vector2 nodeSize = Vector2(GET_NODE_SIZE(graphNode));
			Box nodeBox = Box::pos_size(nodePos, nodeSize);
			if (comment.getCommentBox().IntersectWith(nodeBox))
			{
				std::string name = std::string(TCHAR_TO_UTF8(*graphNode->GetNodeTitle(ENodeTitleType::FullTitle).ToString()));
				int nodeId = nodeIDToNode.Add(graphNode);
				extendedGraph.nodeList.emplace_back(nodeId, name, nodePos, nodeSize, isRerouteFunc(graphNode));
				extendedGraph.selectedNodeIDList.push_back(nodeId);
				AddNodePins(graphNode, extendedGraph.nodeList.back());
			}
		}
	}
}

void ANA_GraphPanel::AddNodeConnectedToAddedNode(ANA_RerouteHelper::IsRerouteFunc isRerouteFunc)
{
	for (size_t i = 0; i < extendedGraph.nodeList.size(); i++)
	{
		extendedGraph.nodeList.reserve(extendedGraph.nodeList.size() + 20);
		auto& node = extendedGraph.nodeList[i];
		node.updatePinIDListArray();
		for (auto pinIDList : node.pinIDListArray)
		{
			for (auto pinID : *pinIDList)
			{
				for (auto otherPin : pinIDToPin[pinID]->LinkedTo)
				{
					UEdGraphNode* connectedToNode = otherPin->GetOwningNode();
					if (nodeIDToNode.Contains(connectedToNode))
						continue;
					AddNode(connectedToNode, isRerouteFunc);
				}
			}
		}
	}
}

void ANA_GraphPanel::AddCommentContainingAddedNode(const TArray<UEdGraphNode_Comment*>& graphCommentList)
{
	for (auto graphComment : graphCommentList)
	{
		if (commentIDToComment.Contains(graphComment))
			continue;
		Vector2 commentPos = Vector2(graphComment->NodePosX, graphComment->NodePosY);
		Vector2 commentSize = Vector2(GET_NODE_SIZE(graphComment));
		Box commentBox = Box::pos_size(commentPos, commentSize);
		for (auto& node : extendedGraph.nodeList)
		{
			if (commentBox.IntersectWith(node.computeNodeBox()))
			{
				AddComment(graphComment);
				break;
			}
		}
	}
}

void ANA_GraphPanel::SetAllConnections()
{
	for (auto& pin : extendedGraph.pinList)
	{
		for (auto otherGraphPin : pinIDToPin[pin.ID]->LinkedTo)
		{
			if (int* otherPinID = pinToPinIDMap.Find(otherGraphPin))
				pin.connectedToPinIDList.push_back(*otherPinID);
		}
	}
}

void ANA_GraphPanel::SetAllNodeInsideComment()
{
	for (auto& comment : extendedGraph.commentList)
	{
		for (auto& node : extendedGraph.nodeList)
		{
			if (comment.getCommentBox().IntersectWith(node.computeNodeBox()))
				comment.insideNodeIDList.push_back(node.ID);
		}
	}
}

void ANA_GraphPanel::SortComment()
{
	// Sort all comment
	std::vector<ExtendedComment*> sortedCommentList;
	for (auto& extendedComment : extendedGraph.commentList)
		sortedCommentList.push_back(&extendedComment);
	std::sort(sortedCommentList.begin(), sortedCommentList.end(),
		[](const ExtendedComment* lhs, const ExtendedComment* rhs)
	{
		return lhs->insideNodeIDList.size() != rhs->insideNodeIDList.size() ? lhs->insideNodeIDList.size() > rhs->insideNodeIDList.size() :
			lhs->pos.y != rhs->pos.y ? lhs->pos.y < rhs->pos.y :
			lhs->pos.x < rhs->pos.x;
	});
	std::vector<std::set<int>> insideNodeIDset;
	for (auto sortedComment : sortedCommentList)
		insideNodeIDset.emplace_back(sortedComment->insideNodeIDList.begin(), sortedComment->insideNodeIDList.end());
	// Distinct or include comments
	for (int i = 0; i < static_cast<int>(sortedCommentList.size()) - 1; i++)
	{
		auto commentI = sortedCommentList[i];
		for (int j = i + 1; j < sortedCommentList.size(); j++)
		{
			auto commentJ = sortedCommentList[j];
			int intersectSize = intersection(insideNodeIDset[i], insideNodeIDset[j]).size();
			if (intersectSize == 0) // distinct
				continue;
			commentI->insideCommentIDList.push_back(commentJ->ID);
			if (intersectSize == insideNodeIDset[j].size()) // already included
				continue;
			for (auto jNodeID : commentJ->insideNodeIDList)
			{
				if (contains(insideNodeIDset[i], jNodeID))
					continue;
				insideNodeIDset[i].insert(jNodeID);
				commentI->insideNodeIDList.push_back(jNodeID);
			}
			for (auto jCommentID : commentJ->insideCommentIDList)
			{
				if (contains(commentI->insideCommentIDList, jCommentID))
					continue;
				commentI->insideCommentIDList.push_back(jCommentID);
			}
			j = i; // reset search
		}
	}
}

void ANA_GraphPanel::ExecuteArrange()
{
#if USE_EXE
	std::ofstream os(export_graph_filename);
	if (os.is_open())
	{
		os << extendedGraph;
		os.close();
		FInteractiveProcess* f = new FInteractiveProcess(arranger_main_filename, "", false);
		f->OnCompleted().BindLambda(
			[this, f](int exit_status, bool b)
		{
			this->isReadyToRead = true;
			delete f;
		});
		/*f->OnOutput().BindLambda(
			[](const FString& output) { DEBUG("%s", *output); }
		);*/
		f->Launch();
	}
	else
	{
		DEBUG("CANNOT EXPORT !!!");
	}
#else
	extendedGraph.nodeList.reserve(extendedGraph.nodeList.size() * 2);
	extendedGraph.pinList.reserve(extendedGraph.pinList.size() * 2);
	extendedGraph.doAll();

	ReadArrangedGraph();
#endif
}

void ANA_GraphPanel::BeginArrangeNodeTransaction()
{
	if (isArrangeNodeTransacting)
		return;
	FString nodeString;
	switch (selectionType)
	{
	case OnlySelected:
	case OnlyComments:
		nodeString = "Arrange Selected Nodes ";
		break;
	case All:
		nodeString = "Arrange All Nodes ";
		break;
	case Connected:
		nodeString = "Arrange Nodes ";
		break;
	default:
		break;
	}
	FString connectModeString = bCompact ? "Compact" : bCenterize ? "Center" : "Straight";
	GEditor->BeginTransaction(*FString("AutoNodeArranger"), FText::FromString(nodeString + connectModeString),
		graphPanel->GetGraphObj());
	graphPanel->GetGraphObj()->Modify();
	isArrangeNodeTransacting = true;
}

void ANA_GraphPanel::EndArrangeNodeTransaction()
{
	if (!isArrangeNodeTransacting)
		return;
	isArrangeNodeTransacting = false;
	GEditor->EndTransaction();
}

void ANA_GraphPanel::ReadArrangedGraph()
{
#if USE_EXE
	isReadyToRead = false;
	std::ifstream is(import_instructions_filename);
	if (is.is_open())
	{
		DEBUG("file opened");
		is >> extendedGraph.instructionGlobalContainer;
		is.close();
		remove(export_graph_filename);
		remove(import_instructions_filename);
#endif
		if (ANA_GraphPanel::instance.extendedGraph.instructionGlobalContainer.allNodeInstructionList.empty())
		{
			// No instruction read --> no arrangement to do
		}
		else
		{
			BeginArrangeNodeTransaction();
			speed = UANA_EditorConfig::Get()->bInstantArrange ? UANA_EditorConfig::instantSpeed : UANA_EditorConfig::normalSpeed;
			rerouteToCreateExecute();
			linkInstructionListExecute();
			commentInstructionListExecute();
		}
#if USE_EXE
	}
	else
	{
		DEBUG("CANNOT IMPORT !!!");
	}
#endif
}

void ANA_GraphPanel::UpdatePosAndSizeForComment(int commentID, Vector2 pos, Vector2 size)
{
	if (!commentIDToComment.IsValidIndex(commentID) || !IsValid(commentIDToComment[commentID]))
		return;
	auto comment = commentIDToComment[commentID];
	comment->Modify();
	comment->NodePosX = pos.x;
	comment->NodePosY = pos.y;
	comment->NodeWidth = size.x;
	comment->NodeHeight = size.y;

	TSharedPtr<SGraphNode> GraphNode = graphPanel->GetNodeWidgetFromGuid(comment->NodeGuid);
	if (GraphNode.IsValid())
	{
		auto GraphNodeComment = StaticCastSharedPtr<SGraphNodeComment>(GraphNode);
		if (GraphNodeComment)
		{
			SGraphNodeComment::FArguments args;
			GraphNodeComment->Construct(args, comment);
		}
	}
}
