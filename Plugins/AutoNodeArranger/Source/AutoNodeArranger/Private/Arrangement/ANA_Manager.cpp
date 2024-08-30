// Copyright 2022 bstt, Inc. All Rights Reserved.

#include "ANA_Manager.h"
#include "../Debug/Log.h"

ANA_Manager ANA_Manager::instance;

#define AUTO_UPDATE true

void ANA_Manager::TickImp(const float DeltaTime)
{
	if (isRegistering)
		RegisterNode();
	if (bAutoArrangeNow)
	{
		ArrangeGraph(bLastCompact, bLastCenterize, lastGraphConfig, false);
		bAutoArrangeNow = false;
	}
#if USE_EXE
	if (ANA_GraphPanel::instance.isReadyToRead)
		ANA_GraphPanel::instance.ReadArrangedGraph();
#endif
	if (!ANA_GraphPanel::instance.isArrangeNodeTransacting)
		return;
#if AUTO_UPDATE
	if (alpha >= 1.f)
	{
		ANA_GraphPanel::instance.FinalizeArrangeGraph();
		ANA_GraphPanel::instance.StopArrangeGraph();
		alpha = 0.f;
		return;
	}
	instructionID = 0;
	alpha = FMath::Clamp(alpha + DeltaTime * ANA_GraphPanel::instance.speed, 0.f, 1.f);
	float lerp = FMath::InterpEaseInOut(0.f, 1.f, alpha, UANA_EditorConfig::exponent);
	ANA_GraphPanel::instance.mergedNodeInstructionListExecute(lerp);
#else
	if (bMerged)
		ANA_GraphPanel::instance.mergedNodeInstructionListExecute(FMath::Min(0.99f, alpha));
	else
		ANA_GraphPanel::instance.allNodeInstructionListExecute(instructionID, alpha);
#endif
}

void ANA_Manager::HandleKeyDownEventImp(const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.IsControlDown() && InKeyEvent.GetKey() == EKeys::Z)
	{
		if (ANA_GraphPanel::instance.isArrangeNodeTransacting)
			ANA_GraphPanel::instance.StopArrangeGraph();
	}
}

void ANA_Manager::HandleKeyUpEventImp(const FKeyEvent& InKeyEvent)
{
}

void ANA_Manager::OnDeselection()
{
	if (bAutoArrangeOnDeselection)
		bAutoArrangeNow = true;
}

void ANA_Manager::ArrangeGraph(bool bCompact, bool bCenterize, const FGraphConfig& graphConfig, bool bAutoArrange)
{
	bLastCompact = bCompact;
	bLastCenterize = bCenterize;
	lastGraphConfig = graphConfig;
	
	bAutoArrangeOnDeselection = bAutoArrange;
	if (bAutoArrange)
	{
		ANA_GraphPanel::UpdateAutoArrangeSelectedNodeSet();
		return;
	}

	if (!bAutoArrangeNow) // clear on manual arrange
		ANA_GraphPanel::ClearAutoArrangeSelectedNodeSet();

	UANA_ProjectConfig::EnqueueGraph(ANA_GraphPanel::instance.graphPanel, UANA_EditorConfig::GetCachedGraphLimit());

	ANA_GraphPanel::instance.ComputeArrangeGraph(bCompact, bCenterize, graphConfig);
}

void ANA_Manager::RegisterAllNodes(SGraphPanel* graphPanel)
{
	registeringGraphPanel = graphPanel;
	RegisterNode();
}

void ANA_Manager::RegisterNode()
{
	static FVector2D viewOffset;
	static float zoomAmount;
	static FGuid bookmarkID;
	static int currentNodeIndex;

	if (!isRegistering) // init registration
	{
		viewOffset = registeringGraphPanel->GetViewOffset();
		zoomAmount = registeringGraphPanel->GetZoomAmount();
		bookmarkID = registeringGraphPanel->GetViewBookmarkId();
		currentNodeIndex = registeringGraphPanel->GetGraphObj()->Nodes.Num() - 1;
		isRegistering = true;
	}

	if (currentNodeIndex + 1 < registeringGraphPanel->GetGraphObj()->Nodes.Num())
	{
		auto oldNode = registeringGraphPanel->GetGraphObj()->Nodes[currentNodeIndex + 1];
		static int registerFailCount = 0;
		if (!UANA_ProjectConfig::RegisterNode(FNamedGuid(registeringGraphPanel), oldNode, registeringGraphPanel) && ++registerFailCount < 10)
			return;
		if (registerFailCount >= 10)
			UE_LOG(LogAutoNodeArranger, Warning, TEXT("too many fails for %s"), *oldNode->GetNodeTitle(ENodeTitleType::FullTitle).ToString());
		registerFailCount = 0;
	}
	if (currentNodeIndex >= 0)
	{
		auto currentNode = registeringGraphPanel->GetGraphObj()->Nodes[currentNodeIndex];
		auto currentNodeRect = FSlateRect::FromPointAndExtent(FVector2D(currentNode->NodePosX, currentNode->NodePosY),
			registeringGraphPanel->GetNodeWidgetFromGuid(currentNode->NodeGuid)->ComputeDesiredSize(1.f));
		registeringGraphPanel->RestoreViewSettings(currentNodeRect.GetCenter() - registeringGraphPanel->GetCachedGeometry().GetLocalSize() * 0.5f, 1.f, bookmarkID);
		currentNodeIndex--;
		return;
	}
	registeringGraphPanel->RestoreViewSettings(viewOffset, zoomAmount, bookmarkID);
	registeringGraphPanel = nullptr;
	isRegistering = false;
}