// Copyright 2022 bstt, Inc. All Rights Reserved.

#include "ANA_NodeSizeHelper.h"

FVector2D ANA_NodeSizeHelper::getNodeSize(UEdGraphNode* node, const FNodePropertiesMap& nodePropertiesMap, SGraphPanel* graphPanel)
{
	if (nodePropertiesMap.IsNodeRegistered(node))
		return nodePropertiesMap.GetNodeProperties(node).GetSize();
	auto n = graphPanel->GetNodeWidgetFromGuid(node->NodeGuid);
	if (!n.IsValid())
		return FVector2D();
	return n->GetDesiredSize();
}

FVector2D ANA_NodeSizeHelper::getPinOffset(UEdGraphNode* node, UEdGraphPin* pin, const FNodePropertiesMap& nodePropertiesMap, SGraphPanel* graphPanel)
{
	if (nodePropertiesMap.IsNodeRegistered(node) && nodePropertiesMap.GetNodeProperties(node).IsPinRegistered(pin))
		return nodePropertiesMap.GetNodeProperties(node).GetPinOffset(pin);
	auto n = graphPanel->GetNodeWidgetFromGuid(node->NodeGuid);
	if (!n.IsValid())
		return FVector2D();
	auto p = n->FindWidgetForPin(pin);
	if (!p.IsValid())
		return FVector2D();
	return p->GetNodeOffset();
}
