// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "CustomObjects/ANA_GraphPanel.h"

/**
 *
 */
class ANA_Manager
{
	// ======== CONSTRUCTORS ========

	//
private:

	// ======== VARIABLES ========

	//
	bool bAutoArrangeOnDeselection = false;
	bool bAutoArrangeNow = false;

	bool isRegistering = false;
	SGraphPanel* registeringGraphPanel = nullptr;

public:

	static ANA_Manager instance;

	bool bLastCompact;
	bool bLastCenterize;
	FGraphConfig lastGraphConfig;

	bool bMerged = false;
	int instructionID = 0.f;
	float alpha = 0.f;

	// ======== DEBUG MOD ========

	//

	// ======== FUNCTIONS ========

	//
public:

	void TickImp(const float DeltaTime);
	void HandleKeyDownEventImp(const FKeyEvent& InKeyEvent);
	void HandleKeyUpEventImp(const FKeyEvent& InKeyEvent);

	void OnDeselection();

	void ArrangeGraph(bool bCompact, bool bCenterize, const FGraphConfig& graphConfig, bool bAutoArrange);
	void RegisterAllNodes(SGraphPanel* graphPanel);

private:
	void RegisterNode();
};
