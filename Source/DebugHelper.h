#pragma once

namespace Debug
{
/**
 * Prints a message to the console and on-screen.
 *
 * @param Msg The message to be printed.
 * @param Color The color of the message.
 * @param InKey The key identifier for the on-screen message.
 *
 * @return None.
 *
 * @throws None.
 */
    static void Print(const FString& Msg, const FColor& Color = FColor::MakeRandomColor(), int32 InKey = -1)
    {
        if(GEngine)
        {
            //on-screen message
            GEngine->AddOnScreenDebugMessage(InKey, 6.0f, Color, Msg);
        }
        //print also in log
        UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
    }
}
