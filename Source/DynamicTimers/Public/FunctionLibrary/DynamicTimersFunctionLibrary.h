// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/DynamicTimersComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DynamicTimersFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DYNAMICTIMERS_API UDynamicTimersFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure, Category= "Dynamic Timers|Time Formatting", meta=(DisplayName= "Format Time to MM:SS"))
	static FText FormatTimeToString(float TotalSeconds);

	UFUNCTION(BlueprintPure, Category= "Dynamic Timers|Components", meta=(DisplayName= "Get Dynamic Timers Component"))
	static UDynamicTimersComponent* GetDynamicTimersComponent(const AGameStateBase* GameState);
};
