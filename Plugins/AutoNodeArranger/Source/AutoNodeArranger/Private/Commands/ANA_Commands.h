// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"

/**
 * 
 */
class ANA_Commands : public TCommands<ANA_Commands>
{
public:
	ANA_Commands();
	virtual ~ANA_Commands() = default;

	virtual void RegisterCommands() override;

	FORCEINLINE static void BindAllCommands() { Get().PrivateBindAllCommands(); }
	FORCEINLINE static void UnbindAllCommands() { Get().PrivateUnbindAllCommands();  }

private:

	void PrivateBindAllCommands() const;
	void PrivateUnbindAllCommands() const;

	TSharedPtr<FUICommandInfo> ArrangeNodesStraight;
	TSharedPtr<FUICommandInfo> ArrangeNodesCompact;
	TSharedPtr<FUICommandInfo> ArrangeNodesCenter;
	TSharedPtr<FUICommandInfo> SelectConnectedGraph;
	TSharedPtr<FUICommandInfo> RegisterAllNodes;
	TSharedPtr<FUICommandInfo> AddCustomConfig;
	TSharedPtr<FUICommandInfo> ToggleAutoArrange;
};
