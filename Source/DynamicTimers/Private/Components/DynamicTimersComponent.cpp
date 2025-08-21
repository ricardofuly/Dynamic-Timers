// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DynamicTimersComponent.h"

#include "Net/UnrealNetwork.h"
#include "Types/DTStructTypes.h"
#include "TimerManager.h"
#include "Engine/World.h"


// Sets default values for this component's properties
UDynamicTimersComponent::UDynamicTimersComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	// ...
}

void UDynamicTimersComponent::RegisterTimer(FGameplayTag TimerTag, float Duration)
{
	if (GetOwnerRole() != ROLE_Authority) return;

	const bool bAlreadyExists = ActiveTimers.ContainsByPredicate([&](const FTimeData& TimerData) {
		return TimerData.TimerTag.MatchesTagExact(TimerTag);
	});

	if (bAlreadyExists) return;

	FTimeData NewTimerData;
	NewTimerData.TimerTag = TimerTag;
	NewTimerData.Duration = Duration;
	//NewTimerData.bPersistForJoinInProgress = bShouldPersist;
	NewTimerData.StartTime = 0.0f;
	NewTimerData.bIsPaused = false;
	NewTimerData.ElapsedTimeWhenPaused = 0.0f;

	ActiveTimers.Add(NewTimerData);

	OnRep_ActiveTimers();
}

void UDynamicTimersComponent::StartGlobalTimer(FGameplayTag TimerTag)
{
	if (GetOwnerRole() != ROLE_Authority) return;

	FTimeData* TimerData = ActiveTimers.FindByPredicate([&](const FTimeData& Data) {
		return Data.TimerTag.MatchesTagExact(TimerTag);
	});

	if (TimerData && TimerData->StartTime == 0.0f)
	{
		TimerData->StartTime = GetWorld()->GetTimeSeconds();

		FTimerHandle ServerTimerHandle;
		FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &UDynamicTimersComponent::OnServerTimerFinished, TimerTag);
		GetWorld()->GetTimerManager().SetTimer(ServerTimerHandle, TimerDelegate, TimerData->Duration, false);

		OnRep_ActiveTimers();
	}
}

void UDynamicTimersComponent::RemoveGlobalTimer(FGameplayTag TimerTag)
{
	if (GetOwnerRole() != ROLE_Authority) return;

	const int32 RemovedCount = ActiveTimers.RemoveAll([&](const FTimeData& TimerData) {
		return TimerData.TimerTag.MatchesTagExact(TimerTag);
	});

	if (RemovedCount > 0)
	{
		OnRep_ActiveTimers();
	}
}

void UDynamicTimersComponent::PauseGlobalTimer(FGameplayTag TimerTag)
{
	if (GetOwnerRole() != ROLE_Authority) return;

	FTimeData* TimerData = ActiveTimers.FindByPredicate([&](const FTimeData& Data) {
		return Data.TimerTag.MatchesTagExact(TimerTag);
	});

	if (TimerData && TimerData->StartTime > 0.0f && !TimerData->bIsPaused)
	{
		TimerData->bIsPaused = true;
		TimerData->ElapsedTimeWhenPaused = GetWorld()->GetTimeSeconds() - TimerData->StartTime;

		OnRep_ActiveTimers();
	}
}

void UDynamicTimersComponent::ResumeGlobalTimer(FGameplayTag TimerTag)
{
	if (GetOwnerRole() != ROLE_Authority) return;

	FTimeData* TimerData = ActiveTimers.FindByPredicate([&](const FTimeData& Data) {
		return Data.TimerTag.MatchesTagExact(TimerTag);
	});

	if (TimerData && TimerData->bIsPaused)
	{
		TimerData->bIsPaused = false;
		TimerData->StartTime = GetWorld()->GetTimeSeconds() - TimerData->ElapsedTimeWhenPaused;

		const float RemainingDuration = TimerData->Duration - TimerData->ElapsedTimeWhenPaused;

		FTimerHandle ServerTimerHandle;
		FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &UDynamicTimersComponent::OnServerTimerFinished, TimerTag);
		GetWorld()->GetTimerManager().SetTimer(ServerTimerHandle, TimerDelegate, RemainingDuration, false);

		OnRep_ActiveTimers();
	}
}

void UDynamicTimersComponent::RegisterGlobalTimers(const TArray<FBulkTimerRegistrationData>& TimersToRegister)
{
	if (GetOwnerRole() != ROLE_Authority || TimersToRegister.IsEmpty()) return; 

	bool bDidChange = false;
	for (const FBulkTimerRegistrationData& RegData : TimersToRegister)
	{
		const bool bAlreadyExists = ActiveTimers.ContainsByPredicate([&](const FTimeData& Timer)
		{
			return Timer.TimerTag.MatchesTagExact(RegData.TimerTag);
		});

		if (!bAlreadyExists && RegData.Duration > 0.0f)
		{
			FTimeData NewTimerData;
			NewTimerData.TimerTag = RegData.TimerTag;
			NewTimerData.Duration = RegData.Duration;
			//NewTimerData.bPersistForJoinInProgress = RegData.bPersistForJoinInProgress;
			NewTimerData.StartTime = 0.0f;

			ActiveTimers.Add(NewTimerData);
			bDidChange = true;
		}
	}

	if (bDidChange)
	{
		OnRep_ActiveTimers();
	}
}

void UDynamicTimersComponent::StartGlobalTimers(const FGameplayTagContainer TimerTagsToStart)
{
	if (GetOwnerRole() != ROLE_Authority || TimerTagsToStart.IsEmpty()) return;

	bool bDidChange = false;
	const float CurrentTime = GetWorld()->GetTimeSeconds();

	for (const FGameplayTag& Tag : TimerTagsToStart.GetGameplayTagArray())
	{
		FTimeData* TimerData = ActiveTimers.FindByPredicate([&](const FTimeData& Timer)
		{
			return Timer.TimerTag.MatchesTagExact(Tag);
		});

		if (TimerData && TimerData->StartTime <= 0.0f)
		{
			TimerData->StartTime = CurrentTime;

			FTimerHandle ServerTimerHandle;
			FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &UDynamicTimersComponent::OnServerTimerFinished, Tag);
			GetWorld()->GetTimerManager().SetTimer(ServerTimerHandle, TimerDelegate, TimerData->Duration, false);
			bDidChange = true;
		}
	}
	if (bDidChange)
	{
		OnRep_ActiveTimers();
	}
}

void UDynamicTimersComponent::PauseGlobalTimers(const FGameplayTagContainer TimerTags)
{
	if (GetOwnerRole() != ROLE_Authority || TimerTags.IsEmpty()) return;

	bool bDidChange = false;
	const float CurrentTime = GetWorld()->GetTimeSeconds();

	for (const FGameplayTag& Tag : TimerTags.GetGameplayTagArray())
	{
		FTimeData* TimerData = ActiveTimers.FindByPredicate([&](const FTimeData& Timer)
		{
			return Timer.TimerTag.MatchesTagExact(Tag);
		});

		TimerData->bIsPaused = true;
		TimerData->ElapsedTimeWhenPaused = CurrentTime - TimerData->StartTime;
		bDidChange = true;
	}
	if (bDidChange)
	{
		OnRep_ActiveTimers();
	}
}

void UDynamicTimersComponent::ResumeGlobalTimers(const FGameplayTagContainer& TimerTags)
{
	if (GetOwnerRole() != ROLE_Authority || TimerTags.IsEmpty()) return;

	bool bDidChange = false;
	const float CurrentTime = GetWorld()->GetTimeSeconds();

	for (const FGameplayTag& Tag : TimerTags.GetGameplayTagArray())
	{
		FTimeData* TimerData = ActiveTimers.FindByPredicate([&](const FTimeData& Timer)
		{
			return Timer.TimerTag.MatchesTagExact(Tag);
		});

		if (TimerData && TimerData->bIsPaused)
		{
			TimerData->bIsPaused = false;
			TimerData->StartTime = CurrentTime - TimerData->ElapsedTimeWhenPaused;

			const float RemainingDuration = TimerData->Duration - TimerData->ElapsedTimeWhenPaused;

			FTimerHandle ServerTimerHandle;
			FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &UDynamicTimersComponent::OnServerTimerFinished, Tag);
			GetWorld()->GetTimerManager().SetTimer(ServerTimerHandle, TimerDelegate, RemainingDuration, false);
			bDidChange = true;
		}
	}
	if (bDidChange)
	{
		OnRep_ActiveTimers();
	}
}

bool UDynamicTimersComponent::GetTimerRemainingTime(FGameplayTag TimerTag, float& RemainingTime) const
{
	const FTimeData* Timer = ActiveTimers.FindByPredicate([&](const FTimeData& Data) {
		return Data.TimerTag.MatchesTagExact(TimerTag);
	});

	if (Timer)
	{
		if (Timer->StartTime == 0.0f)
		{
			RemainingTime = Timer->Duration;
			return true;
		}
		
		if (Timer->bIsPaused)
		{
			RemainingTime = FMath::Max(0.0f, Timer->Duration - Timer->ElapsedTimeWhenPaused);
			return true;
		}

		const float ElapsedTime = GetWorld()->GetTimeSeconds() - Timer->StartTime;
		RemainingTime = FMath::Max(0.0f, Timer->Duration - ElapsedTime);
		return true;
	}
	RemainingTime = 0.0f;
	return false;
}

void UDynamicTimersComponent::OnRep_ActiveTimers()
{
	for (const FTimeData& NewTimer : ActiveTimers)
	{
		const FTimeData* OldTimer = Old_ActiveTimers.FindByPredicate([&](const FTimeData& Data) {
			return Data.TimerTag.MatchesTagExact(NewTimer.TimerTag);
		});

		if (!OldTimer)
		{
			OnTimerRegistered.Broadcast(NewTimer.TimerTag, NewTimer.Duration);
		}
		else
		{
			if (NewTimer.StartTime > 0.0f && OldTimer->StartTime == 0.0f)
			{
				OnTimerStarted.Broadcast(NewTimer.TimerTag, NewTimer.Duration);
			}
			else if (NewTimer.bIsPaused && !OldTimer->bIsPaused)
			{
				OnTimerPaused.Broadcast(NewTimer.TimerTag);
			}
			else if (!NewTimer.bIsPaused && OldTimer->bIsPaused)
			{
				OnTimerResumed.Broadcast(NewTimer.TimerTag);
			}
		}
	}

	for (const FTimeData& OldTimer : Old_ActiveTimers)
	{
		const bool bStillExists = ActiveTimers.ContainsByPredicate([&](const FTimeData& Data)
		{
			return Data.TimerTag.MatchesTagExact(OldTimer.TimerTag);
		});

		if (!bStillExists)
		{
			OnTimerFinished.Broadcast(OldTimer.TimerTag);
		}
	}

	Old_ActiveTimers = ActiveTimers;
}

void UDynamicTimersComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UDynamicTimersComponent, ActiveTimers);
}

void UDynamicTimersComponent::OnServerTimerFinished(FGameplayTag TimerTag)
{
	RemoveGlobalTimer(TimerTag);
}


// Called when the game starts
void UDynamicTimersComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

