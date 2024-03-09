// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "KrakenHUD.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;
struct FWidgetControllerParams;
class UOverlayWidgetController;
class UKrakenUserWidget;
/**
 * 
 */
UCLASS()
class HEARTWEAVER_API AKrakenHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	TObjectPtr<UKrakenUserWidget> OverlayWidget;

	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& InParams);

	void InitOverlay(APlayerController* InPlayerController, APlayerState* InPlayerState, UAbilitySystemComponent* InAbilitySystemComponent, UAttributeSet* InAttributeSet);

	UFUNCTION(BlueprintCallable, Category = "Widgets")
	void ClearTemporaryWidgets();

private:

	UPROPERTY(EditAnywhere)
	TSubclassOf<UKrakenUserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;
	
};
