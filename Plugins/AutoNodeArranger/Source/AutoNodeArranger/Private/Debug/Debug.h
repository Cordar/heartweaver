// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Log.h"

// ---------- debug macros --------------

FString int_to_tab_FString(int i);

static int debug_count = 0;

#define __FILENAME__ FString(strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define DEBUG(text, ...) { debug_count++; FString message_ = FString::Printf(TEXT("%s::%d : " __FUNCTION__ " : " text), *__FILENAME__, __LINE__, __VA_ARGS__); \
	UE_LOG(LogAutoNodeArranger, Warning, TEXT("%s"), *message_) }
#define DEBUGT(tab_count, text, ...) { debug_count++; UE_LOG(LogAutoNodeArranger, Warning, TEXT("%s%d - line %d : " text), *int_to_tab_FString(tab_count), debug_count, __LINE__, __VA_ARGS__) }
#define GET_NODE_TITLE(node) node->GetNodeTitle(ENodeTitleType::FullTitle).ToString()
#define GET_PIN_NAME(pin) pin->PinName.ToString()
