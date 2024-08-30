// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "ANA_ProjectConfig.h"

void UANA_ProjectConfig::PostInitProperties()
{
	Super::PostInitProperties();

	for (auto nodePropertiesMap : nodePropertiesMapMap)
		PrivateEnqueueGraph(nodePropertiesMap.Key, nodePropertiesMapMap.Num());
}

void UANA_ProjectConfig::PrivateEnqueueGraph(FNamedGuid namedGuid, int32 maxQueueSize)
{
	graphQueue.Remove(namedGuid);
	graphQueue.Insert(namedGuid, 0);
	while (graphQueue.Num() > maxQueueSize)
		nodePropertiesMapMap.Remove(graphQueue.Pop());		
}
