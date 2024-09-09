#pragma once

#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"
#include "EnemyState.generated.h"

UENUM(BlueprintType, DisplayName = "Enemy State")
enum class EEnemyState : uint8
{
	DEACTIVATED UMETA(DisplayName = "Deactivated"),
	// IDLE UMETA(DisplayName = "Idle"),
	WAITING UMETA(DisplayName = "Waiting"),
	PATROLLING UMETA(DisplayName = "Patrolling"),
	STATIC_PATROLLING UMETA(DisplayName = "Static Patrolling"),
	MOVING_TO_TARGET UMETA(DisplayName = "Moving To Target"),
	MOVING_TO_LOCATION UMETA(DisplayName = "Moving To Location")
};

//* Esto estaba interesante pero realmente no vale la pena
// UCLASS(EditInlineNew, meta = (DisplayName = "Enemy State Enum"))
// class HEARTWEAVER_API UBlackboardKeyType_EnemyStateEnum : public UBlackboardKeyType_Enum
// {
// 	GENERATED_BODY()
//
// public:
// 	UBlackboardKeyType_EnemyStateEnum()
// 	{
// 		EnumType = StaticEnum<EEnemyState>(); // Asignar el tipo de enum
// 	}
// };