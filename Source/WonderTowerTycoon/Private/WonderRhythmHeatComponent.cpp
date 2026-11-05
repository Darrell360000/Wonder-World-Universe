// Copyright Epic Games, Inc. All Rights Reserved.

#include "WonderRhythmHeatComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

UWonderRhythmHeatComponent::UWonderRhythmHeatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	CurrentRhythmHeat = 0.0f;
	MaxRhythmHeat = 100.0f;
	HeatDecayRate = 2.5f;
	BaseShockwaveRadius = 1500.0f;
}

void UWonderRhythmHeatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWonderRhythmHeatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentRhythmHeat > 0.0f)
	{
		CurrentRhythmHeat = FMath::Clamp(CurrentRhythmHeat - (HeatDecayRate * DeltaTime), 0.0f, MaxRhythmHeat);
		OnRhythmHeatChanged.Broadcast(CurrentRhythmHeat, MaxRhythmHeat);
	}
}

void UWonderRhythmHeatComponent::AddRhythmHeat(float Amount)
{
	if (Amount <= 0.0f) return;

	CurrentRhythmHeat = FMath::Clamp(CurrentRhythmHeat + Amount, 0.0f, MaxRhythmHeat);
	OnRhythmHeatChanged.Broadcast(CurrentRhythmHeat, MaxRhythmHeat);
}

bool UWonderRhythmHeatComponent::UnleashCymaticShockwave()
{
	if (CurrentRhythmHeat < 25.0f)
	{
		return false;
	}

	float Radius = BaseShockwaveRadius;
	if (IsOverdriveReady())
	{
		Radius *= OverdriveMultiplier;
	}

	CurrentRhythmHeat = 0.0f;
	OnRhythmHeatChanged.Broadcast(CurrentRhythmHeat, MaxRhythmHeat);
	OnCymaticShockwaveTriggered.Broadcast(Radius);

	return true;
}

void UWonderRhythmHeatComponent::PurifyDistrict(const FString& DistrictName, float Radius)
{
	float PurifiedAmount = 15.0f;
	OnDistrictRoughnessPurified.Broadcast(DistrictName, PurifiedAmount);
}
