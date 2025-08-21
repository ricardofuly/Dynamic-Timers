// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibrary/DynamicTimersFunctionLibrary.h"

#include "GameFramework/GameStateBase.h"

FText UDynamicTimersFunctionLibrary::FormatTimeToString(float TotalSeconds)
{
	const float SafeTotalSeconds = FMath::Max(0.0f, TotalSeconds);

	const int32 TotalSecondsInt = FMath::FloorToInt(SafeTotalSeconds);
	const int32 Minutes = TotalSecondsInt / 60;
	const int32 Seconds = TotalSecondsInt % 60;

	FNumberFormattingOptions FormattingOptions;
	FormattingOptions.MinimumIntegralDigits = 2;
	FormattingOptions.MaximumIntegralDigits = 2;

	FFormatNamedArguments Args;
	Args.Add(TEXT("Minutes"), FText::AsNumber(Minutes, &FormattingOptions));
	Args.Add(TEXT("Seconds"), FText::AsNumber(Seconds, &FormattingOptions));

	return FText::Format(NSLOCTEXT("MyGame", "TimeFormat", "{Minutes}:{Seconds}"), Args);
}

UDynamicTimersComponent* UDynamicTimersFunctionLibrary::GetDynamicTimersComponent(const AGameStateBase* GameState)
{
	if (GameState)
	{
		return GameState->GetComponentByClass<UDynamicTimersComponent>();
	}
	return nullptr;
}
