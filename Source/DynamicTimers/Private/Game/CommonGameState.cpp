// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/CommonGameState.h"

ACommonGameState::ACommonGameState()
{
	DynamicTimersComponent = CreateDefaultSubobject<UDynamicTimersComponent>(TEXT("DynamicTimersComponent"));
}
