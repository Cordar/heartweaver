// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "../Config/NodePropertiesMap.h"
#include "EdGraph/EdGraphNode.h"
#include "SGraphPanel.h"

#define GET_NODE_SIZE(node) ANA_NodeSizeHelper::getNodeSize(node, nodePropertiesMap, graphPanel)
#define GET_PIN_OFFSET(node,pin) ANA_NodeSizeHelper::getPinOffset(node, pin, nodePropertiesMap, graphPanel)

class ANA_NodeSizeHelper
{
	ANA_NodeSizeHelper() = delete;

public:
	static FVector2D getNodeSize(UEdGraphNode* node, const FNodePropertiesMap& nodePropertiesMap, SGraphPanel* graphPanel);
	static FVector2D getPinOffset(UEdGraphNode* node, UEdGraphPin* pin, const FNodePropertiesMap& nodePropertiesMap, SGraphPanel* graphPanel);
};
