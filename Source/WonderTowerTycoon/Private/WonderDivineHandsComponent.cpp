// Copyright Epic Games, Inc. All Rights Reserved.

#include "WonderDivineHandsComponent.h"

UWonderDivineHandsComponent::UWonderDivineHandsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	GridCellSize = 800.0f;
	FloorHeight = 400.0f;
	bEtherealHandsActive = true;
}

void UWonderDivineHandsComponent::BeginPlay()
{
	Super::BeginPlay();
	OnEtherealHandsStateChanged.Broadcast(bEtherealHandsActive);
}

void UWonderDivineHandsComponent::SetEtherealHandsActive(bool bActive)
{
	bEtherealHandsActive = bActive;
	OnEtherealHandsStateChanged.Broadcast(bEtherealHandsActive);
}

bool UWonderDivineHandsComponent::SnapTowerModuleToGrid(FName ModuleType, FVector WorldPosition, FVector& OutSnappedPosition)
{
	if (!bEtherealHandsActive)
	{
		return false;
	}

	float SnappedX = FMath::RoundToFloat(WorldPosition.X / GridCellSize) * GridCellSize;
	float SnappedY = FMath::RoundToFloat(WorldPosition.Y / GridCellSize) * GridCellSize;
	float SnappedZ = FMath::RoundToFloat(WorldPosition.Z / FloorHeight) * FloorHeight;

	OutSnappedPosition = FVector(SnappedX, SnappedY, SnappedZ);
	OnTowerModuleSnapped.Broadcast(ModuleType, OutSnappedPosition, true);

	return true;
}
