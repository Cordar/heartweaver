// Copyright 2022 bstt, Inc. All Rights Reserved.


#include "ANA_Commands.h"
#include "ANA_Style.h"
#include "../Helpers/ANA_CommandHelper.h"
#include "Interfaces/IMainFrameModule.h"

ANA_Commands::ANA_Commands() : TCommands<ANA_Commands>
(
	"AutoNodeArranger",
	NSLOCTEXT("AutoNodeArranger", "AutoNodeArrangerCommand", "Auto Node Arranger"),
	NAME_None,
	ANA_Style::Get().GetStyleSetName()
)
{
}

#define LOCTEXT_NAMESPACE "ANA_Commands"

void ANA_Commands::RegisterCommands()
{
	// shift, ctrl, alt, cmd
	UI_COMMAND(ArrangeNodesStraight, "Arrange Nodes Straight", "Arrange Nodes with Straight mode", EUserInterfaceActionType::Button,
		FInputChord(EKeys::Q, true, false, false, false));
	UI_COMMAND(ArrangeNodesCenter, "Arrange Nodes Center", "Arrange Nodes with Center mode", EUserInterfaceActionType::Button,
		FInputChord(EKeys::X, true, false, false, false));
	UI_COMMAND(ArrangeNodesCompact, "Arrange Nodes Compact", "Arrange Nodes with Compact mode", EUserInterfaceActionType::Button,
		FInputChord(EKeys::V, true, false, false, false));
	UI_COMMAND(SelectConnectedGraph, "Select Connected Graph", "Select all connected nodes", EUserInterfaceActionType::Button,
		FInputChord(EKeys::F, true, false, false, false));
	UI_COMMAND(RegisterAllNodes, "Register All Nodes", "Register all nodes of the current graph", EUserInterfaceActionType::Button,
		FInputChord(EKeys::P, true, false, false, false));
	UI_COMMAND(AddCustomConfig, "Add Custom Config", "Add custom config for the current graph type", EUserInterfaceActionType::Button,
		FInputChord(EKeys::Enter, true, false, false, false));
	UI_COMMAND(ToggleAutoArrange, "Toggle Auto Arrange", "Enable/Disable the auto arrange option", EUserInterfaceActionType::Button,
		FInputChord(EKeys::SpaceBar, true, false, false, false));
}

void ANA_Commands::PrivateBindAllCommands() const
{
	IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
	const TSharedRef<FUICommandList>& CommandBindings = MainFrame.GetMainFrameCommandBindings();
	CommandBindings->MapAction(ArrangeNodesStraight, FExecuteAction::CreateStatic(ANA_CommandHelper::ArrangeNodesStraight));
	CommandBindings->MapAction(ArrangeNodesCenter, FExecuteAction::CreateStatic(ANA_CommandHelper::ArrangeNodesCenter));
	CommandBindings->MapAction(ArrangeNodesCompact, FExecuteAction::CreateStatic(ANA_CommandHelper::ArrangeNodesCompact));
	CommandBindings->MapAction(SelectConnectedGraph, FExecuteAction::CreateStatic(ANA_CommandHelper::SelectConnectedGraph));
	CommandBindings->MapAction(RegisterAllNodes, FExecuteAction::CreateStatic(ANA_CommandHelper::RegisterAllNodes));
	CommandBindings->MapAction(AddCustomConfig, FExecuteAction::CreateStatic(ANA_CommandHelper::AddCustomConfig));
	CommandBindings->MapAction(ToggleAutoArrange, FExecuteAction::CreateStatic(ANA_CommandHelper::ToggleAutoArrange));
}

void ANA_Commands::PrivateUnbindAllCommands() const
{
	IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
	const TSharedRef<FUICommandList>& CommandBindings = MainFrame.GetMainFrameCommandBindings();
	CommandBindings->UnmapAction(ArrangeNodesStraight);
	CommandBindings->UnmapAction(ArrangeNodesCenter);
	CommandBindings->UnmapAction(ArrangeNodesCompact);
	CommandBindings->UnmapAction(SelectConnectedGraph);
	CommandBindings->UnmapAction(RegisterAllNodes);
	CommandBindings->UnmapAction(AddCustomConfig);
	CommandBindings->UnmapAction(ToggleAutoArrange);
}

#undef LOCTEXT_NAMESPACE

