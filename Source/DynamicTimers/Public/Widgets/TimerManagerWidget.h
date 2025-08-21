// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimerDisplayWidget.h"
#include "Blueprint/UserWidget.h"
#include "Game/CommonGameState.h"
#include "GameFramework/GameState.h"
#include "TimerManagerWidget.generated.h"

/**
 * 
 */
UCLASS()
class DYNAMICTIMERS_API UTimerManagerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* TimersContainer;

	UPROPERTY(EditDefaultsOnly, Category="Dynamic Timers | Widgets")
	TSubclassOf<UTimerDisplayWidget> TimerDisplayWidgetClass;

private:

	UFUNCTION()
	void HandleTimerRegistered(FGameplayTag TimerTag, float Duration);

	UFUNCTION()
	void HandleTimerStarted(FGameplayTag TimerTag, float Duration);

	UFUNCTION()
	void HandleTimerFinished(FGameplayTag TimerTag);

	UFUNCTION()
	void HandleTimerPaused(FGameplayTag TimerTag);

	UFUNCTION()
	void HandleTimerResumed(FGameplayTag TimerTag);

	UPROPERTY()
	TMap<FGameplayTag, UTimerDisplayWidget*> ActiveTimerWidgets;

	UPROPERTY()
	TObjectPtr<AGameStateBase> CachedGameState;

	UPROPERTY(EditAnywhere, Category="Dynamic Timers | Settings")
	FGameplayTag FilterTimerTag;
};
