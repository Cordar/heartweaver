// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "KrakenInputConfig.h"
#include "KrakenInputComponent.generated.h"

class UEnhancedInputLocalPlayerSubsystem;

/**
 * 
 */
UCLASS(Config = Input)
class HEARTWEAVER_API UKrakenInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:

	UKrakenInputComponent();

	void AddInputMappings(const UKrakenInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	void RemoveInputMappings(const UKrakenInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;

	template<class UserClass, typename FuncType>
	void BindNativeAction(const UKrakenInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType
	                      Func, bool bLogIfNotFound);
	
	void RemoveBinds(TArray<uint32>& BindHandles);
};

template<class UserClass, typename FuncType>
void UKrakenInputComponent::BindNativeAction(const UKrakenInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType
                                          Func, bool bLogIfNotFound)
{
	check(InputConfig);
	if (const UInputAction* Ia = InputConfig->FindInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(Ia, TriggerEvent, Object, Func);
	}
}
