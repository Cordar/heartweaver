// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "SGraphPanel.h"
#include "NamedGuid.generated.h"

USTRUCT()
struct FNamedGuid
{
	GENERATED_BODY()
	
	// do not use
	FNamedGuid() {}
	explicit FNamedGuid(UEdGraphPin* pin) : name(pin->GetName()), sGuid(pin->PersistentGuid.ToString()) { }
	explicit FNamedGuid(UEdGraphNode* node) : name(node->GetNodeTitle(ENodeTitleType::FullTitle).ToString()), sGuid(node->NodeGuid.ToString()) { }
	explicit FNamedGuid(SGraphPanel* graphPanel) : name(graphPanel->GetGraphObj()->GetFullGroupName(true)),
		sGuid(graphPanel->GetGraphObj()->GraphGuid.ToString()) { }

	UPROPERTY(VisibleAnywhere, Category = "Named GUID")
		FString name; // only informative 
	UPROPERTY()
		FString sGuid; // used as key 

	friend bool operator==(const FNamedGuid& lhs, const FNamedGuid& rhs) { return lhs.sGuid == rhs.sGuid; }
	friend uint32 GetTypeHash(const FNamedGuid& nameGuid) { return  GetTypeHash(nameGuid.sGuid); }
};
