// Copyright 2022 bstt, Inc. All Rights Reserved.

#include "ANA_InputProcessor.h"
#include "../Helpers/ANA_CommandHelper.h"
#include "../Arrangement/ANA_Manager.h"
#include "../Config/ANA_ProjectConfig.h"
#include "../Debug/Debug.h"
#include "Input/Events.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Layout/WidgetPath.h"
#include "Widgets/Notifications/SNotificationList.h"

TSharedPtr<ANA_InputProcessor> ANA_InputProcessor::instance = nullptr;

void ANA_InputProcessor::RegisterProcessor()
{
	instance = MakeShareable(new ANA_InputProcessor());
	if (FSlateApplication::IsInitialized())
		FSlateApplication::Get().RegisterInputPreProcessor(instance);
}

void ANA_InputProcessor::UnregisterProcessor()
{
	if (FSlateApplication::IsInitialized())
		FSlateApplication::Get().UnregisterInputPreProcessor(instance);
	instance.Reset();
}

void ANA_InputProcessor::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{
	ANA_Manager::instance.TickImp(DeltaTime);
	
	AutoRegisterNodes(DeltaTime);
	UpdatePinLinkedTo();
	CheckDeselection();
}

bool ANA_InputProcessor::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	if (!MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
		return false; // false to not consume event
	FWidgetPath widgetsUnderCursor = SlateApp.LocateWindowUnderMouse(MouseEvent.GetScreenSpacePosition(), SlateApp.GetInteractiveTopLevelWindows());
	FScopedSwitchWorldHack SwitchWorld(widgetsUnderCursor);

	bControlDown = MouseEvent.GetModifierKeys().IsControlDown();
	UpdateCurrentGraphAndPin(widgetsUnderCursor);
	
	return false; // false to not consume event
}

bool ANA_InputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	ANA_Manager::instance.HandleKeyDownEventImp(InKeyEvent);
	return false; // false to not consume event
}

bool ANA_InputProcessor::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	ANA_Manager::instance.HandleKeyUpEventImp(InKeyEvent);
	return false; // false to not consume event
}

void ANA_InputProcessor::AutoRegisterNodes(const float DeltaTime)
{
	if (!currentWeakSGraphPanel.IsValid())
		return;
	SGraphPanel* graphPanel = currentWeakSGraphPanel.Pin().Get();

	updateNodeSizeTime -= DeltaTime;
	if (updateNodeSizeTime < 0.f)
	{
		for (auto graphNode : graphPanel->GetGraphObj()->Nodes)
		{
			FVector2D pos = FVector2D(graphNode->NodePosX, graphNode->NodePosY);
			FVector2D size = FVector2D(graphNode->NodeWidth, graphNode->NodeHeight);
			FSlateRect graphRect = FSlateRect::FromPointAndExtent(pos, size);
			if (graphPanel->IsRectVisible(graphRect.GetTopLeft(), graphRect.GetBottomRight()))
				UANA_ProjectConfig::RegisterNode(currentNamedGuid, graphNode, graphPanel);
		}
		UANA_ProjectConfig::Get()->SaveConfig();
		updateNodeSizeTime = 1.f; // auto registering nodes each second
	}
}

void ANA_InputProcessor::UpdatePinLinkedTo()
{
	if (!UANA_EditorConfig::Get()->bAutoArrange)
		return;
	if (!currentWeakSGraphPanel.IsValid())
		return;
	SGraphPanel* graphPanel = currentWeakSGraphPanel.Pin().Get();

	if (!currentGraphPin.IsValid())
	{
		currentGraphPin = nullptr;
		return;
	}
	auto graphPin = currentGraphPin.Pin()->GetPinObj();
	if (graphPin->LinkedTo != oldLinkedTo)
	{
		if (bControlDown)
		{
			oldLinkedTo = graphPin->LinkedTo;
			bControlDown = false;
			return;
		}
			countdownTickCount--;
		if (countdownTickCount == 0)
		{
			FGraphPanelSelectionSet nodeToSelectSet;
			for (auto connectedToPin : graphPin->LinkedTo)
				nodeToSelectSet.Add(connectedToPin->GetOwningNodeUnchecked());
			for (auto oldConnectedToPin : oldLinkedTo)
				nodeToSelectSet.Remove(oldConnectedToPin->GetOwningNodeUnchecked());
			nodeToSelectSet.Add(graphPin->GetOwningNodeUnchecked());
			graphPanel->SelectionManager.SetSelectionSet(nodeToSelectSet);
			ANA_CommandHelper::AutoArrangeNodes();
			graphPanel->SelectionManager.ClearSelectionSet();
			graphPanel->SelectionManager.SetNodeSelection(graphPin->GetOwningNodeUnchecked(), true);
			oldLinkedTo = graphPin->LinkedTo;
		}
		else if (countdownTickCount < 0)
			countdownTickCount = 2;
	}
}

void ANA_InputProcessor::UpdateCurrentGraphAndPin(const FWidgetPath& widgetsUnderCursor)
{
	if (!UANA_EditorConfig::Get()->bAutoArrange)
		return;
	TWeakPtr<SGraphPanel> weakSGraphPanel = nullptr;
	TWeakPtr<SGraphPin> weakGraphPin = nullptr;
	for (int i = 0; i <= widgetsUnderCursor.Widgets.Num() - 1; i++)
	{
		FString widgetName = widgetsUnderCursor.Widgets[i].Widget->GetTypeAsString();
		if (widgetName == "SGraphPanel")
			weakSGraphPanel = StaticCastSharedRef<SGraphPanel>(widgetsUnderCursor.Widgets[i].Widget);
		if (widgetName.Contains("GraphPin") || widgetName.Contains("BehaviorTreePin") || widgetName.Contains("OutputPin") ||
			widgetName.Contains("SEnvironmentQueryPin"))
			weakGraphPin = StaticCastSharedRef<SGraphPin>(widgetsUnderCursor.Widgets[i].Widget);
	}
	if (weakSGraphPanel.IsValid())
	{
		currentWeakSGraphPanel = weakSGraphPanel;
		currentNamedGuid = FNamedGuid(currentWeakSGraphPanel.Pin().Get());

		if (weakGraphPin.IsValid())
		{
			currentGraphPin = weakGraphPin;
			if (!currentGraphPin.IsValid())
			{
				currentGraphPin = nullptr;
				return;
			}
			UEdGraphPin* graphPin = currentGraphPin.Pin()->GetPinObj();
			if (!graphPin)
			{
				currentGraphPin = nullptr;
				return;
			}
			if ((bControlDown || graphPin->bOrphanedPin) && graphPin->LinkedTo.Num() > 0)
				graphPin = graphPin->LinkedTo[0];
			else
				bControlDown = false;

			oldLinkedTo.Empty();
			if (graphPin->bOrphanedPin)
				currentGraphPin = nullptr;
			else
			{
				oldLinkedTo.Append(graphPin->LinkedTo);
			}
		}
	}
}

void ANA_InputProcessor::CheckDeselection()
{
	if (!currentWeakSGraphPanel.IsValid())
		return;
	SGraphPanel* graphPanel = currentWeakSGraphPanel.Pin().Get();
	if (graphPanel->SelectionManager.SelectedNodes.Num() == 0)
	{
		if (!oldIsDeselected)
		{
			oldIsDeselected = true;
			ANA_Manager::instance.OnDeselection();
		}
	}
	else
		oldIsDeselected = false;
}