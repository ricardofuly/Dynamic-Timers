#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DTStructTypes.generated.h"

USTRUCT(BlueprintType)
struct FTimeData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dynamic Timers")
	FGameplayTag TimerTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dynamic Timers")
	float StartTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dynamic Timers")
	float Duration = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dynamic Timers")
	bool bIsPaused = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dynamic Timers")
	float ElapsedTimeWhenPaused = 0.0f;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dynamic Timers")
	//bool bPersistForJoinInProgress = true;

	bool operator==(const FTimeData& Other) const
	{
		return TimerTag == Other.TimerTag;
	}
	
};

USTRUCT(BlueprintType)
struct FBulkTimerRegistrationData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Timers")
	FGameplayTag TimerTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Timers")
	float Duration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Timers")
	bool bPersistForJoinInProgress = true;
};
