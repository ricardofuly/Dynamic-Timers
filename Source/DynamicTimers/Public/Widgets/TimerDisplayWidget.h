// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "TimerDisplayWidget.generated.h"

/**
 * 
 */
UCLASS()
class DYNAMICTIMERS_API UTimerDisplayWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	void SetTimerTag(FGameplayTag InTag);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dynamic Timers")
	void OnTimerActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dynamic Timers")
	void OnTimerDeactivated();

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Dynamic Timers")
	FGameplayTag TimerTag;

};
