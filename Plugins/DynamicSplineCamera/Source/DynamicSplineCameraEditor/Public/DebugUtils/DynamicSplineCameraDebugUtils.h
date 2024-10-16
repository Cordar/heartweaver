#pragma once

#include "Misc/MessageDialog.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"
/*
// #define print(Text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT(Text), false)
// #define print_k(Key, Text) if (GEngine) GEngine->AddOnScreenDebugMessage(Key, 2.0f, FColor::White, TEXT(Text), false)

// Example:	printf("float: %f", 5.0f);
// #define printf(Format, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, FString::Printf(TEXT(Format), ##__VA_ARGS__), false)
// #define printf_k(Key, Format, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(Key, 2.0f, FColor::White, FString::Printf(TEXT(Format), ##__VA_ARGS__), false)

// Example: printc("Hola", FColor::White); 
// #define printc(Text, Color) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, Color, TEXT(Text), false)
// #define printc_k(Key, Text, Color) if (GEngine) GEngine->AddOnScreenDebugMessage(Key, 2.0f, Color, TEXT(Text), false)

/* Prints de textos sin argumentos#1#
#define PRINT_1_ARGS(Text)  if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT(Text), false)
#define PRINT_2_ARGS(Text, Key)  if (GEngine) GEngine->AddOnScreenDebugMessage(Key, 2.0f, FColor::White,TEXT(Text), false)
#define PRINT_3_ARGS(Text, Key, Color)  if (GEngine) GEngine->AddOnScreenDebugMessage(Key, 2.0f, Color, TEXT(Text), false)
#define PRINT_4_ARGS(Text, Key, Color, Time)  if (GEngine) GEngine->AddOnScreenDebugMessage(Key, Time, Color, TEXT(Text), false)

#define GET_PRINT_5TH_ARG(arg1, arg2, arg3, arg4, arg5, ...) arg5

#define PRINT_STRING_MACRO_CHOOSER(...) \
GET_PRINT_5TH_ARG(__VA_ARGS__,PRINT_4_ARGS,PRINT_3_ARGS, \
PRINT_2_ARGS, PRINT_1_ARGS, )

#define PRINT(...) PRINT_STRING_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

/* Prints de textos con argumentos#1#
#define PRINTF_1_ARGS(Format, ...)  if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, FString::Printf(TEXT(Format), ##__VA_ARGS__), false)
#define PRINTF_2_ARGS(Format, Key, ...)  if (GEngine) GEngine->AddOnScreenDebugMessage(Key, 2.0f, FColor::White,FString::Printf(TEXT(Format), ##__VA_ARGS__), false)
#define PRINTF_3_ARGS(Format, Key, Color, ...)  if (GEngine) GEngine->AddOnScreenDebugMessage(Key, 2.0f, Color, FString::Printf(TEXT(Format), ##__VA_ARGS__), false)
#define PRINTF_4_ARGS(Format, Key, Color, Time, ...)  if (GEngine) GEngine->AddOnScreenDebugMessage(Key, Time, Color, FString::Printf(TEXT(Format), ##__VA_ARGS__), false)

/*
 *  EJEMPLOS DE PRINTS:
*  
 PRINT("Holaa");
 PRINT("Holaa2", 2);
 PRINT("Holaa3", -1, FColor::Black);
 PRINT("Holaa3", -1, FColor::Green, 9);

 PRINTF_1_ARGS("float: %f", 5.0f);
 *
 * 
 #1#
 */

namespace DynamicSplineCameraDebugUtils
{
	static void Print(const FString& Message, const FColor& Color)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 8.0f, Color, Message);
		}
	}


	static EAppReturnType::Type ShowMsgDialog(EAppMsgType::Type MsgType, const FString& Message,
	                                          bool bShowMsgAsWarning = true)
	{
		if (bShowMsgAsWarning)
		{
			FText MsgTitle = FText::FromString(TEXT("Warning"));
			return FMessageDialog::Open(MsgType, FText::FromString(Message), &MsgTitle);
		}
		else
		{
			return FMessageDialog::Open(MsgType, FText::FromString(Message));
		}
	}

	static void ShowNotifyInfo(const FString& Message)
	{
		FNotificationInfo NotifyInfo(FText::FromString(Message));
		NotifyInfo.bUseLargeFont = true;
		NotifyInfo.FadeOutDuration = 7.f;
		FSlateNotificationManager::Get().AddNotification(NotifyInfo);
	}
}
