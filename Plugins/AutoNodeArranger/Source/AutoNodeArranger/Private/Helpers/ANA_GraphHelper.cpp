// Copyright 2022 bstt, Inc. All Rights Reserved.

#include "ANA_GraphHelper.h"
#include "../Config/ANA_EditorConfig.h"
#include "../Debug/Log.h"

bool ANA_GraphHelper::GetValidGraph(SGraphPanel*& graphPanel, FGraphConfig& graphConfig, EGraphTypeEnum& graphType, bool& isForbiddenGraph)
{
	TSharedPtr<SGraphPanel> shrGraphPanel = GetCurrentGraphPanel();
	if (!shrGraphPanel.IsValid())
		return false;
	auto graphClassName = shrGraphPanel->GetGraphObj()->GetClass()->GetFName().ToString();
	graphPanel = shrGraphPanel.Get();
	graphConfig = UANA_EditorConfig::ComputeGraphConfig(graphClassName, graphType, isForbiddenGraph);
	return true;
}

TSharedPtr<SGraphPanel> ANA_GraphHelper::GetCurrentGraphPanel()
{
	auto activeTab = FGlobalTabmanager::Get()->GetActiveTab();
	if (!activeTab.IsValid())
		return nullptr;
	TSharedRef<SWidget> sWidget = activeTab->GetContent();
	return RecursiveGetCurrentGraphPanel(sWidget);
}

TSharedPtr<SGraphPanel> ANA_GraphHelper::RecursiveGetCurrentGraphPanel(TSharedRef<SWidget> sWidget)
{
	if (sWidget->GetTypeAsString() == "SGraphPanel")
		return TSharedPtr<SGraphPanel>(StaticCastSharedRef<SGraphPanel>(sWidget));
	FChildren* children = sWidget->GetChildren();
	for (size_t i = 0; i < children->Num(); i++)
	{
		auto result = RecursiveGetCurrentGraphPanel(children->GetChildAt(i));
		if (result.IsValid())
			return result;
	}
	return nullptr;
}
