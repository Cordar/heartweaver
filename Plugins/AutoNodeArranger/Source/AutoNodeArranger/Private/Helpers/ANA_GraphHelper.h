// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "SGraphPanel.h"

#define INIT_GRAPH_PANEL() \
SGraphPanel* graphPanel; \
FGraphConfig graphConfig; \
EGraphTypeEnum graphType; \
bool isForbiddenGraph; \
if (!ANA_GraphHelper::GetValidGraph(graphPanel, graphConfig, graphType, isForbiddenGraph)) \
	return

struct FGraphConfig;

enum class EGraphTypeEnum : uint8;

class ANA_GraphHelper
{
	ANA_GraphHelper() = delete;

public:
	static bool GetValidGraph(SGraphPanel*& graphPanel, FGraphConfig& graphConfig, EGraphTypeEnum& graphType, bool& isForbiddenGraph);
	static TSharedPtr<SGraphPanel> GetCurrentGraphPanel();

private:
	static TSharedPtr<SGraphPanel> RecursiveGetCurrentGraphPanel(TSharedRef<SWidget> sWidget);
}; // ANA_GraphHelper
