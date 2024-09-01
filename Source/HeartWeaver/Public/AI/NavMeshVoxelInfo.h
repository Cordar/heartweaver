#pragma once

#include "CoreMinimal.h"
#include "NavMeshVoxelInfo.generated.h"

UENUM(BlueprintType)
enum class EVoxelValue : uint8 {
	AVAIBLE = 0,
	BLOCKED = 1,
};

USTRUCT(BlueprintType)
struct FNavMeshVoxelInfo
{
	GENERATED_BODY()

	unsigned char Value = 0;
	
	// Podemos guardar cualquier otra información del voxel aquí
};
