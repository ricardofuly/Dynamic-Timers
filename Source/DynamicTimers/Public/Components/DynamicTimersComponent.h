// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "DynamicTimersComponent.generated.h"

struct FBulkTimerRegistrationData;
struct FTimeData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOntimerRegistered, FGameplayTag, TimerTag, float, Duration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOntimerStarted, FGameplayTag, TimerTag, float, Duration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOntimerFinished, FGameplayTag, TimerTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOntimerPaused, FGameplayTag, TimerTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOntimerResumed, FGameplayTag, TimerTag);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DYNAMICTIMERS_API UDynamicTimersComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDynamicTimersComponent();

	UFUNCTION(BlueprintCallable, Category="Dynamic Timers|Time Formatting")
	void RegisterTimer(FGameplayTag TimerTag, float Duration);

	UFUNCTION(BlueprintCallable, Category= "Dynamic Timers", meta=(BlueprintAuthorityOnly))
	void StartGlobalTimer(FGameplayTag TimerTag);
	
	UFUNCTION(BlueprintCallable, Category= "Dynamic Timers", meta=(BlueprintAuthorityOnly))
	void RemoveGlobalTimer(FGameplayTag TimerTag);
	
	UFUNCTION(BlueprintCallable, Category= "Dynamic Timers", meta=(BlueprintAuthorityOnly))
	void PauseGlobalTimer(FGameplayTag TimerTag);
	
	UFUNCTION(BlueprintCallable, Category= "Dynamic Timers", meta=(BlueprintAuthorityOnly))
	void ResumeGlobalTimer(FGameplayTag TimerTag);

	// Bulk Timer Functions
	UFUNCTION(BlueprintCallable, Category= "Dynamic Timers", meta=(BlueprintAuthorityOnly))
	void RegisterGlobalTimers(const TArray<FBulkTimerRegistrationData>& TimersToRegister);

	UFUNCTION(BlueprintCallable, Category="Dynamic Timers", meta=(BlueprintAuthorityOnly))
	void StartGlobalTimers(const FGameplayTagContainer TimerTagsToStart);

	UFUNCTION(BlueprintCallable, Category="Dynamic Timers", meta=(BlueprintAuthorityOnly))
	void PauseGlobalTimers(const FGameplayTagContainer TimerTags);

	UFUNCTION(BlueprintCallable, Category="Dynamic Timers", meta=(BlueprintAuthorityOnly))
	void ResumeGlobalTimers(const FGameplayTagContainer& TimerTags);
	
	// Get the remaining time for a specific timer
	UFUNCTION(BlueprintPure, Category="Dynamic Timers")
	bool GetTimerRemainingTime(FGameplayTag TimerTag, float& RemainingTime) const;


	// Dynamic Delegates
	// These are used to notify when a timer is registered, started, finished, paused, resumed
	UPROPERTY(BlueprintAssignable, Category="Dynamic Timers")
	FOntimerRegistered OnTimerRegistered;
	
	UPROPERTY(BlueprintAssignable, Category="Dynamic Timers")
	FOntimerStarted OnTimerStarted;

	UPROPERTY(BlueprintAssignable, Category="Dynamic Timers")
	FOntimerFinished OnTimerFinished;

	UPROPERTY(BlueprintAssignable, Category="Dynamic Timers")
	FOntimerPaused OnTimerPaused;

	UPROPERTY(BlueprintAssignable, Category="Dynamic Timers")
	FOntimerResumed OnTimerResumed;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(ReplicatedUsing= OnRep_ActiveTimers)
	TArray<FTimeData> ActiveTimers;

	UFUNCTION()
	void OnRep_ActiveTimers();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

	void OnServerTimerFinished(FGameplayTag TimerTag);

	TArray<FTimeData> Old_ActiveTimers;
};
