// Copyright Kraken's Den Studios. All rights reserved.


#include "UI/Widget/KrakenUserWidget.h"

void UKrakenUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
