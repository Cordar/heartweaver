// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EdGraphNode_Comment.h"
#include "NamedGuid.h"
#include "NodePropertiesMap.generated.h"

USTRUCT()
struct FNodeProperties
{
	GENERATED_BODY()

public:
	// do not use
	FNodeProperties() {}

	FNodeProperties(UEdGraphNode* graphNode, SGraphPanel* graphPanel)
	{
		TSharedPtr<SGraphNode> n = graphPanel->GetNodeWidgetFromGuid(graphNode->NodeGuid);
		if (!n.IsValid())
			return;
		nodeSize = n->GetDesiredSize();
		if (nodeSize == FVector2D::ZeroVector)
			return;
		pinOffsetMap.Reset();
		for (auto pin : graphNode->Pins)
		{
			auto p = n->FindWidgetForPin(pin);
			if (!p.IsValid())
				continue;
			pinOffsetMap.Add(pin->GetName(), p->GetNodeOffset());
		}
		isValid = true;
		zoomAmount = FMath::Min(1.f, graphPanel->GetZoomAmount()); // standard size is for zoom amount equals to 1.f
	}

	FORCEINLINE bool IsPinRegistered(UEdGraphPin* graphPin) const { return pinOffsetMap.Contains(graphPin->GetName()); }
	FORCEINLINE FVector2D GetPinOffset(UEdGraphPin* graphPin) const { return pinOffsetMap[graphPin->GetName()]; } // need to check if pin is registered
	FORCEINLINE FVector2D GetSize() const { return nodeSize; }
	FORCEINLINE float GetZoomAmount() const { return zoomAmount; }
	FORCEINLINE bool GetIsValid() const { return isValid; }

private:
	bool isValid = false; // NodeProerties valid only if node found and size valid
	
	UPROPERTY(EditAnywhere, Category = "Node Properties")
		FVector2D nodeSize;
	UPROPERTY(EditAnywhere, Category = "Node Properties")
		float zoomAmount = 0.f;
	UPROPERTY(EditAnywhere, Category = "Node Properties")
		TMap<FString, FVector2D> pinOffsetMap; // key is pin name
};

USTRUCT()
struct FNodePropertiesMap
{
	GENERATED_BODY()

public:
	bool RegisterNode(UEdGraphNode* graphNode, SGraphPanel* graphPanel)
	{
		if (IsValid(graphNode) && !graphNode->GetClass()->IsChildOf<UEdGraphNode_Comment>())
		{
			FNamedGuid graphNodeNamedGuid = FNamedGuid(graphNode);
			FNodeProperties* oldNodeProperties = nodeSizeMap.Find(graphNodeNamedGuid);
			if (oldNodeProperties && oldNodeProperties->GetZoomAmount() > graphPanel->GetZoomAmount()) // already registered with a better zoom
				return true;
			FNodeProperties nodeProperties = FNodeProperties(graphNode, graphPanel);
			if (!nodeProperties.GetIsValid())
				return false;
			nodeSizeMap.Add(graphNodeNamedGuid, nodeProperties);
		}
		return true;
	}
	FORCEINLINE bool IsNodeRegistered(UEdGraphNode* graphNode) const { return nodeSizeMap.Contains(FNamedGuid(graphNode)); }
	FORCEINLINE FNodeProperties GetNodeProperties(UEdGraphNode* graphNode) const { return nodeSizeMap[FNamedGuid(graphNode)]; } // need to check if node is registered

private:
	UPROPERTY(EditAnywhere, Category = "Node Size Map")
		TMap<FNamedGuid, FNodeProperties> nodeSizeMap;
};