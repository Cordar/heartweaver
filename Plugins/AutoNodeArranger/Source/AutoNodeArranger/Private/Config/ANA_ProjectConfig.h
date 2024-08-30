// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EdGraph/EdGraph.h"
#include "NodePropertiesMap.h"
#include "SGraphPanel.h"
#include "ANA_ProjectConfig.generated.h"

/**
 * 
 */
UCLASS(config = Project)
class UANA_ProjectConfig : public UObject
{
	GENERATED_BODY()
	
public:

	virtual void PostInitProperties() override;

	FORCEINLINE static UANA_ProjectConfig* Get() {	return GetMutableDefault<UANA_ProjectConfig>(); }

	FORCEINLINE static void EnqueueGraph(SGraphPanel* graphPanel, int32 maxQueueSize) { Get()->PrivateEnqueueGraph(FNamedGuid(graphPanel), maxQueueSize); }

	FORCEINLINE static FNodePropertiesMap GetNodePropertiesMap(SGraphPanel* graphPanel) { return Get()->nodePropertiesMapMap.FindRef(FNamedGuid(graphPanel)); }
	FORCEINLINE static bool RegisterNode(FNamedGuid namedGuid, UEdGraphNode* graphNode, SGraphPanel* graphPanel)
		{ return Get()->nodePropertiesMapMap.FindOrAdd(namedGuid).RegisterNode(graphNode, graphPanel); }

private:
	
	UPROPERTY(EditAnywhere, config, Category = "Auto Node Arranger", DisplayName = "Node Properties Register")
		TMap<FNamedGuid, FNodePropertiesMap> nodePropertiesMapMap; // key is panelGuid

	TArray<FNamedGuid> graphQueue;

	void PrivateEnqueueGraph(FNamedGuid namedGuid, int32 maxQueueSize);
};
