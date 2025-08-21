// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/DynamicTimersComponent.h"
#include "GameFramework/GameStateBase.h"
#include "CommonGameState.generated.h"

/**
 * 
 */
UCLASS()
class DYNAMICTIMERS_API ACommonGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	ACommonGameState();

protected:

	UPROPERTY()
	UDynamicTimersComponent* DynamicTimersComponent;
};
