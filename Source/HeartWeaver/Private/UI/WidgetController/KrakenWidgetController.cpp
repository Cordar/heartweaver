// Copyright Kraken's Den Studios. All rights reserved.


#include "UI/WidgetController/KrakenWidgetController.h"

void UKrakenWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& InParams)
{
	PlayerController = InParams.PlayerController;
	PlayerState = InParams.PlayerState;
	AbilitySystemComponent = InParams.AbilitySystemComponent;
	AttributeSet = InParams.AttributeSet;
}

void UKrakenWidgetController::BroadcastInitialValues()
{
}

void UKrakenWidgetController::BindCallbacksToDependencies()
{
}
