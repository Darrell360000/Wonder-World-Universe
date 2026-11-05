// Copyright Epic Games, Inc. All Rights Reserved.

#include "WonderIronRiffComponent.h"

UWonderIronRiffComponent::UWonderIronRiffComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	CurrentPressure = 0.0f;
	MaxPressure = 100.0f;
	BaseSlamRadius = 800.0f;
	BaseKnockbackImpulse = 2500.0f;
}

void UWonderIronRiffComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWonderIronRiffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Slow passive pressure bleed if not maintained
	if (CurrentPressure > 0.0f)
	{
		CurrentPressure = FMath::Clamp(CurrentPressure - (1.0f * DeltaTime), 0.0f, MaxPressure);
		OnAnvilHydraulicPressureChanged.Broadcast(CurrentPressure);
	}
}

void UWonderIronRiffComponent::ChargeHydraulicPiston(float DeltaPressure)
{
	if (DeltaPressure <= 0.0f) return;

	CurrentPressure = FMath::Clamp(CurrentPressure + DeltaPressure, 0.0f, MaxPressure);
	OnAnvilHydraulicPressureChanged.Broadcast(CurrentPressure);
}

bool UWonderIronRiffComponent::ExecuteSubbassGroundSlam(FVector ImpactPoint)
{
	if (CurrentPressure < 20.0f)
	{
		return false;
	}

	float PowerScalar = CurrentPressure / MaxPressure;
	float Radius = BaseSlamRadius * (1.0f + PowerScalar);
	float Impulse = BaseKnockbackImpulse * (1.0f + (PowerScalar * 1.5f));

	CurrentPressure = 0.0f;
	OnAnvilHydraulicPressureChanged.Broadcast(CurrentPressure);
	OnSubbassGroundSlam.Broadcast(ImpactPoint, Radius, Impulse);

	return true;
}
