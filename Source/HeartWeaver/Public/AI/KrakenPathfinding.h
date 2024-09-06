// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"

struct FNode;

/**
 * 
 */
class HEARTWEAVER_API KrakenPathfinding
{
public:
	KrakenPathfinding();
	~KrakenPathfinding();

	static TArray<FVector> GetPath(FVector StartPoint, FVector EndPoint, float GridDistance, TSet<FVector> *GridMap, TSet<FVector>* BlockedGridMap = nullptr);
	static TArray<FVector> ReconstructPath(FNode* EndNode);

	static TArray<FVector> GetNeighbors(FVector CurrentPosition, float GridDistance, TSet<FVector>* GridMap, TSet<FVector>* BlockedGridMap);

	static void ReverseArray(TArray<FVector>& Array);
};
