// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/TimerManagerWidget.h"

#include "Components/DynamicTimersComponent.h"
#include "Components/PanelWidget.h"

void UTimerManagerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UWorld* World = GetWorld())
	{
		CachedGameState = World->GetGameState<AGameStateBase>();
		if (CachedGameState)
		{
			CachedGameState->FindComponentByClass<UDynamicTimersComponent>()->OnTimerRegistered.AddDynamic(this, &UTimerManagerWidget::HandleTimerRegistered);
			CachedGameState->FindComponentByClass<UDynamicTimersComponent>()->OnTimerStarted.AddDynamic(this, &UTimerManagerWidget::HandleTimerStarted);
			CachedGameState->FindComponentByClass<UDynamicTimersComponent>()->OnTimerFinished.AddDynamic(this, &UTimerManagerWidget::HandleTimerFinished);
			CachedGameState->FindComponentByClass<UDynamicTimersComponent>()->OnTimerPaused.AddDynamic(this, &UTimerManagerWidget::HandleTimerPaused);
			CachedGameState->FindComponentByClass<UDynamicTimersComponent>()->OnTimerResumed.AddDynamic(this, &UTimerManagerWidget::HandleTimerResumed);
		}
	}
}

void UTimerManagerWidget::NativeDestruct()
{
	if (CachedGameState)
	{
		if (UDynamicTimersComponent* TimersComponent = CachedGameState->FindComponentByClass<UDynamicTimersComponent>())
		{
			TimersComponent->OnTimerRegistered.RemoveDynamic(this, &UTimerManagerWidget::HandleTimerRegistered);
			TimersComponent->OnTimerStarted.RemoveDynamic(this, &UTimerManagerWidget::HandleTimerStarted);
			TimersComponent->OnTimerFinished.RemoveDynamic(this, &UTimerManagerWidget::HandleTimerFinished);
			TimersComponent->OnTimerPaused.RemoveDynamic(this, &UTimerManagerWidget::HandleTimerPaused);
			TimersComponent->OnTimerResumed.RemoveDynamic(this, &UTimerManagerWidget::HandleTimerResumed);
		}
	}
	
	Super::NativeDestruct();
}

void UTimerManagerWidget::HandleTimerRegistered(FGameplayTag TimerTag, float Duration)
{
	const bool bPassesFilter = !FilterTimerTag.IsValid() || TimerTag.MatchesTagExact(FilterTimerTag);
	if (!bPassesFilter) return;
	
	if (!TimersContainer || !TimerDisplayWidgetClass) return;

	if (UTimerDisplayWidget* NewWidget = CreateWidget<UTimerDisplayWidget>(this, TimerDisplayWidgetClass))
	{
		NewWidget->SetTimerTag(TimerTag);
		TimersContainer->AddChild(NewWidget);
		ActiveTimerWidgets.Add(TimerTag, NewWidget);

		NewWidget->OnTimerDeactivated();
	}
}

void UTimerManagerWidget::HandleTimerStarted(FGameplayTag TimerTag, float Duration)
{
	if (UTimerDisplayWidget** FoundWidget = ActiveTimerWidgets.Find(TimerTag))
	{
		(*FoundWidget)->OnTimerActivated();
	}
}

void UTimerManagerWidget::HandleTimerFinished(FGameplayTag TimerTag)
{
	UTimerDisplayWidget* FoundWidget = nullptr;
	if (ActiveTimerWidgets.RemoveAndCopyValue(TimerTag, FoundWidget))
	{
		if (FoundWidget)
		{
			FoundWidget->RemoveFromParent();
		}
	}
}

void UTimerManagerWidget::HandleTimerPaused(FGameplayTag TimerTag)
{
	if (UTimerDisplayWidget** FoundWidget = ActiveTimerWidgets.Find(TimerTag))
	{
		(*FoundWidget)->OnTimerDeactivated();
	}
}

void UTimerManagerWidget::HandleTimerResumed(FGameplayTag TimerTag)
{
	if (UTimerDisplayWidget** FoundWidget = ActiveTimerWidgets.Find(TimerTag))
	{
		(*FoundWidget)->OnTimerActivated();
	}
}
