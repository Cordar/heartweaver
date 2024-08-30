// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EdGraph/EdGraph.h"
#include "ANA_EditorConfig.generated.h"

USTRUCT(BlueprintType)
struct FGraphConfig
{
	GENERATED_BODY()

	// do not use
	FGraphConfig() {}

	FGraphConfig(FVector2D Spacing,
		FVector2D CompactSpacing,
		FVector2D CommentSpacing) :
		Spacing(Spacing),
		CompactSpacing(CompactSpacing),
		CommentSpacing(CommentSpacing) {}

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Graph Config")
		FVector2D Spacing;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Graph Config")
		FVector2D CompactSpacing;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Graph Config")
		FVector2D CommentSpacing;
};

USTRUCT()
struct FGraphTypeStr
{
	GENERATED_BODY()

	// do not use
	FGraphTypeStr() {}

	explicit FGraphTypeStr(FString value) : value(value) {}

	UPROPERTY(VisibleAnywhere, DisplayName = "GraphType", Category = "Graph Config")
		FString value;

	friend bool operator==(const FGraphTypeStr& graphType1, const FGraphTypeStr& graphType2) { return graphType1.value == graphType2.value; }
	friend uint32 GetTypeHash(const FGraphTypeStr& graphType) { return  GetTypeHash(graphType.value); }
};

UENUM()
enum class EGraphTypeEnum : uint8
{
	Blueprint,
	Material,
	AI,
	Sound
};

UENUM()
enum class EArrangeSelectionType : uint8
{
	// one selected node --> arrange connected graph
	// multiple selected nodes --> arrange only selected nodes
	OneForAll UMETA(DisplayName = "One For All"),
	// always arrange only selected nodes
	AlwaysSelected UMETA(DisplayName = "Always Selected"),
	// always arrange connected graph
	AlwaysAll UMETA(DisplayName = "Always All"),
};

/**
 * 
 */
UCLASS(config = Editor)
class UANA_EditorConfig : public UObject
{
	GENERATED_BODY()
	
public:

	FORCEINLINE static UANA_EditorConfig* Get() {	return GetMutableDefault<UANA_EditorConfig>(); }

	UPROPERTY(EditAnywhere, config, Category = "Auto Node Arranger")
		EArrangeSelectionType arrangeSelectionType = EArrangeSelectionType::OneForAll; 
	
	UPROPERTY(EditAnywhere, config, Category = "Auto Node Arranger")
		bool bAutoArrange = true;

	UPROPERTY(EditAnywhere, config, Category = "Auto Node Arranger")
		bool bAutoGenerateReroute = true;

	// Special X spacing for exec blueprint
	UPROPERTY(EditAnywhere, config, Category = "Auto Node Arranger")
		float ExecSpacingX = 120.f;

	UPROPERTY(EditAnywhere, config, Category = "Auto Node Arranger")
		bool bGroupAllConnectedGraph = true;

	UPROPERTY(EditAnywhere, config, Category = "Auto Node Arranger")
		bool bInstantArrange = false;

	UPROPERTY(EditAnywhere, config, Category = "Auto Node Arranger")
		bool bProgressiveSelection = true;

	// Offset added to handle overlap
	UPROPERTY(EditAnywhere, config, Category = "Auto Node Arranger|Advanced", DisplayName = "Overlap Offset")
		float LineWidth = 5.f;

	static const float exponent;

	static const float normalSpeed;
	static const float instantSpeed;

	FORCEINLINE static FGraphConfig ComputeGraphConfig(FString graphTypeString, EGraphTypeEnum& graphType, bool& isForbiddenGraph)
		{ return Get()->privateComputeGraphConfig(graphTypeString, graphType, isForbiddenGraph); }

	FORCEINLINE static bool ContainsGraphConfig(FString graphTypeString) { return Get()->customGraphConfigMap.Contains(FGraphTypeStr(graphTypeString)); }
	FORCEINLINE static FGraphConfig GetGraphConfig(FString graphTypeString) { return Get()->customGraphConfigMap[FGraphTypeStr(graphTypeString)]; }
	FORCEINLINE static void AddGraphConfig(FString graphTypeString) { Get()->customGraphConfigMap.Add(FGraphTypeStr(graphTypeString), Get()->DefaultGraphConfig); }
	FORCEINLINE static int32 GetCachedGraphLimit() { return Get()->cachedGraphLimit; }

	FORCEINLINE static bool CheckShowNewFeatures()
	{
		return false;
		/*
		bool result = Get()->bShowPoll;
		Get()->bShowPoll = false;
		return result;
		*/
	}
	
private:

	UPROPERTY(EditAnywhere, config, Category = "Auto Node Arranger")
		FGraphConfig DefaultGraphConfig = FGraphConfig(FVector2D(50.f, 30.f), FVector2D(30.f, 15.f), FVector2D(24.f, 24.f));

	UPROPERTY(EditAnywhere, config, Category = "Auto Node Arranger")
		FGraphConfig MaterialGraphConfig = FGraphConfig(FVector2D(100.f, 50.f), FVector2D(50.f, 20.f), FVector2D(30.f, 30.f));

	UPROPERTY(EditAnywhere, config, Category = "Auto Node Arranger")
		FGraphConfig AI_GraphConfig = FGraphConfig(FVector2D(70.f, 100.f), FVector2D(30.f, 50.f), FVector2D(35.f, 35.f));

	// Number of cached graphs limited in order to save memory
	UPROPERTY(EditAnywhere, config, Category = "Auto Node Arranger|Advanced", meta = (UIMin = "2", UIMax = "15", ClampMin = "2", ClampMax = "25"))
		int32 cachedGraphLimit = 10;

	UPROPERTY(EditAnywhere, config, Category = "Auto Node Arranger|Advanced", DisplayName = "Custom Graph Configs")
		TMap<FGraphTypeStr, FGraphConfig> customGraphConfigMap;

	/*
	// Show the poll for filling it
	UPROPERTY(EditAnywhere, config, Category = "Auto Node Arranger")
		bool bShowPoll = false;
	*/

	TSet<FString> ForbiddenGraphTypeSet = { "AnimationStateMachineGraph", "EdGraph_ReferenceViewer" };

	TSet<FString> AI_GraphTypeSet = { "BehaviorTreeGraph", "EnvironmentQueryGraph" };

	FGraphConfig privateComputeGraphConfig(FString graphTypeString, EGraphTypeEnum& graphType, bool& isForbiddenGraph)
	{
		FGraphConfig result;
		if (AI_GraphTypeSet.Contains(graphTypeString))
		{
			graphType = EGraphTypeEnum::AI;
			result = AI_GraphConfig;
		}
		else if (graphTypeString == "MaterialGraph")
		{
			graphType = EGraphTypeEnum::Material;
			result = MaterialGraphConfig;
		}
		else if (graphTypeString == "SoundCueGraph")
		{
			graphType = EGraphTypeEnum::Sound;
			result = DefaultGraphConfig;
		}
		else
		{
			graphType = EGraphTypeEnum::Blueprint;
			result = DefaultGraphConfig;
		}
		isForbiddenGraph = ForbiddenGraphTypeSet.Contains(graphTypeString);
		return ContainsGraphConfig(graphTypeString) ? GetGraphConfig(graphTypeString) : result;
	}

};
