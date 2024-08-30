// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "../Basic/Vector2.h"

#include "../CppCommon.h"

struct CppGraphConfig
{
	enum class GraphType
	{
		E_BLUEPRINT,
		E_MATERIAL,
		E_AI,
		E_SOUND,
	};

// ======== ATTRIBUTES ========
	
	// graphh config

	float execSpacingX = 0.f;

	Vector2 spacing;
	Vector2 commentSpacing;

	// editor config

	bool bAutoGenerateReroute = false;
	bool bGroupAllConnectedGraph = false;

	float lineWidth = 0.f;

	// arrangement config

	bool bCompact = false;
	bool bCenterize = false;
	GraphType graphType = GraphType::E_BLUEPRINT;

// ======== CONSTRUCTOR ========

	CppGraphConfig() : CppGraphConfig(0.f, Vector2(), Vector2(), false, false, 0.f, false, false, GraphType::E_BLUEPRINT) {}
	CppGraphConfig(float _execSpacingX, Vector2 _spacing, Vector2 _commentSpacing, bool _bAutoGenerateReroute, bool _bGroupAllConnectedGraph, float _lineWidth,
		bool _bCompact, bool _bCenterize, GraphType _graphType) :
		execSpacingX(_execSpacingX), spacing(_spacing), commentSpacing(_commentSpacing), bAutoGenerateReroute(_bAutoGenerateReroute),
		bGroupAllConnectedGraph(_bGroupAllConnectedGraph), lineWidth(_lineWidth), bCompact(_bCompact), bCenterize(_bCenterize), graphType(_graphType) {}

// ======== METHODS ========

	bool GetIsBlueprintGraph() const
	{
		return graphType == GraphType::E_BLUEPRINT;
	}

	bool GetIsMaterialGraph() const
	{
		return graphType == GraphType::E_MATERIAL;
	}

	bool GetIsAI_Graph() const
	{
		return graphType == GraphType::E_AI;
	}

	bool GetIsSoundGraph() const
	{
		return graphType == GraphType::E_SOUND;
	}

	bool GetIsCompactBlueprintGraph() const
	{
		return bCompact && GetIsBlueprintGraph();
	}

	bool GetIsMaterialOrSoundGraph() const
	{
		return GetIsMaterialGraph() || GetIsSoundGraph();
	}

// ======== READ/WRITE ========

	friend std::istream& operator>>(std::istream& is, CppGraphConfig& graphConfig)
	{
		is >> graphConfig.execSpacingX >> graphConfig.spacing >> graphConfig.commentSpacing
			>> graphConfig.bAutoGenerateReroute >> graphConfig.bGroupAllConnectedGraph >> graphConfig.lineWidth >> graphConfig.bCompact >> graphConfig.bCenterize;
		read_enum(is, graphConfig.graphType);
		return is;
	}

	friend std::ostream& operator<<(std::ostream& os, const CppGraphConfig& graphConfig)
	{
		os << graphConfig.execSpacingX << " " << graphConfig.spacing << " "
			<< graphConfig.commentSpacing << " " << graphConfig.bAutoGenerateReroute << " "
			<< graphConfig.bGroupAllConnectedGraph << " " << graphConfig.lineWidth << " "
			<< graphConfig.bCompact << " " << graphConfig.bCenterize << " ";
		write_enum(os, graphConfig.graphType);
		return os;
	}
};