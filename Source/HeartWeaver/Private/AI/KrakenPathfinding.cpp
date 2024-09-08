// Copyright Kraken's Den Studios. All rights reserved.


#include "AI/KrakenPathfinding.h"

struct FNode
{
	FVector Position; // La posición del nodo en el espacio 3D
	float GCost; // El costo desde el punto de inicio hasta este nodo
	float HCost; // El costo estimado desde este nodo hasta el punto final (heurística)
	float FCost; // El costo total (GCost + HCost)
	FNode* Parent; // Nodo padre para reconstruir el camino

	FNode(FVector InPosition)
		: Position(InPosition), GCost(0), HCost(0), FCost(0), Parent(nullptr)
	{
	}
};

KrakenPathfinding::KrakenPathfinding()
{
}

KrakenPathfinding::~KrakenPathfinding()
{
}

TArray<FVector> KrakenPathfinding::GetPath(FVector StartPoint, FVector EndPoint, float GridDistance,
                                           TSet<FVector>* GridMap, TSet<FVector>* BlockedGridMap)
{
	TArray<FNode*> OpenSet;
	TSet<FVector> ClosedSet;

	// Nodo inicial
	FNode* StartNode = new FNode(StartPoint);
	StartNode->HCost = FVector::Dist(StartPoint, EndPoint);
	StartNode->FCost = StartNode->GCost + StartNode->HCost;

	OpenSet.Add(StartNode);

	while (OpenSet.Num() > 0)
	{
		// Ordenar OpenSet por FCost (usando el operador '<' en FNode)
		OpenSet.Sort([](const FNode& A, const FNode& B)
		{
			return A.FCost < B.FCost;
		});

		FNode* CurrentNode = OpenSet[0];
		OpenSet.RemoveAt(0);

		// Si hemos llegado al nodo final, reconstruimos el camino
		if (CurrentNode->Position.Equals(EndPoint, 1.0f)) // Tolerancia de 1.0f
		{
			return ReconstructPath(CurrentNode);
		}

		ClosedSet.Add(CurrentNode->Position);

		// Buscar los nodos vecinos
		TArray<FVector> Neighbors = GetNeighbors(CurrentNode->Position, GridDistance, GridMap, BlockedGridMap);

		for (const FVector& NeighborPos : Neighbors)
		{
			if (ClosedSet.Contains(NeighborPos))
			{
				continue; // Nodo ya evaluado
			}

			float TentativeGCost = CurrentNode->GCost + FVector::Dist(CurrentNode->Position, NeighborPos);

			FNode* NeighborNode = nullptr;

			// Buscar si ya existe en el OpenSet
			for (FNode* Node : OpenSet)
			{
				if (Node->Position.Equals(NeighborPos, 1.0f))
				{
					NeighborNode = Node;
					break;
				}
			}

			if (NeighborNode == nullptr)
			{
				NeighborNode = new FNode(NeighborPos);
				OpenSet.Add(NeighborNode);
			}
			else if (TentativeGCost >= NeighborNode->GCost)
			{
				continue; // Este no es un mejor camino
			}

			// Este es el mejor camino conocido hasta ahora
			NeighborNode->Parent = CurrentNode;
			NeighborNode->GCost = TentativeGCost;
			NeighborNode->HCost = FVector::Dist(NeighborPos, EndPoint);
			NeighborNode->FCost = NeighborNode->GCost + NeighborNode->HCost;
		}
	}

	// Si no se encuentra un camino
	return TArray<FVector>();
}

TArray<FVector> KrakenPathfinding::ReconstructPath(FNode* EndNode)
{
	TArray<FVector> Path;
	FNode* CurrentNode = EndNode;

	while (CurrentNode != nullptr)
	{
		Path.Add(CurrentNode->Position);
		CurrentNode = CurrentNode->Parent;
	}

	ReverseArray(Path);
	return Path;
}

TArray<FVector> KrakenPathfinding::GetNeighbors(FVector CurrentPosition, float GridDistance, TSet<FVector>* GridMap,
                                                TSet<FVector>* BlockedGridMap)
{
	bool ConsultBlockedGridMap = (BlockedGridMap != nullptr);
	// bool ConsultBlockedGridMap = false;

	TArray<FVector> Neighbors;

	// Definir las posibles direcciones para vecinos en 3D
	TArray<FVector> Directions = {
		FVector(GridDistance, 0, 0),
		FVector(-GridDistance, 0, 0),
		FVector(0, GridDistance, 0),
		FVector(0, -GridDistance, 0),
		FVector(0, 0, GridDistance),
		FVector(0, 0, -GridDistance)
	};

	for (const FVector& Direction : Directions)
	{
		FVector NeighborPosition = CurrentPosition + Direction;

		if (!ConsultBlockedGridMap)
		{
			if (GridMap->Contains(NeighborPosition))
			{
				Neighbors.Add(NeighborPosition);
			}
		}
		else
		{
			if (GridMap->Contains(NeighborPosition) && !BlockedGridMap->Contains(NeighborPosition))
			{
				Neighbors.Add(NeighborPosition);
			}
		}
	}

	return Neighbors;
}

void KrakenPathfinding::ReverseArray(TArray<FVector>& Array)
{
	int32 StartIndex = 0;
	int32 EndIndex = Array.Num() - 1;

	while (StartIndex < EndIndex)
	{
		Array.Swap(StartIndex, EndIndex);
		++StartIndex;
		--EndIndex;
	}
}
