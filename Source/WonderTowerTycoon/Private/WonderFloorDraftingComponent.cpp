#include "WonderFloorDraftingComponent.h"
#include "Kismet/GameplayStatics.h"

UWonderFloorDraftingComponent::UWonderFloorDraftingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWonderFloorDraftingComponent::GenerateDraftChoices(int32 CurrentFloor, TArray<EFloorRoomType>& OutChoices)
{
	OutChoices.Empty();

	if (CurrentFloor >= 5)
	{
		// Top Floor always drafts the Penthouse CEO Boss
		OutChoices.Add(EFloorRoomType::PenthouseBoss);
		return;
	}

	// Procedural 3-card draft choices
	OutChoices.Add(EFloorRoomType::SecurityBarracks);
	OutChoices.Add(EFloorRoomType::ServerVault);
	OutChoices.Add(EFloorRoomType::ExecutiveOffice);
}

void UWonderFloorDraftingComponent::SelectAndBuildFloor(int32 FloorNumber, EFloorRoomType SelectedType)
{
	FDraftableFloorNode NewNode;
	NewNode.FloorLevel = FloorNumber;
	NewNode.RoomType = SelectedType;
	NewNode.bIsCleared = false;

	ActiveTowerFloors.Add(NewNode);
	CurrentAscentFloor = FloorNumber;

	// Broadcast sound sting on floor creation
	UGameplayStatics::PlaySound2D(GetWorld(), nullptr);
}

void UWonderFloorDraftingComponent::MarkFloorCleared(int32 FloorNumber)
{
	for (FDraftableFloorNode& Node : ActiveTowerFloors)
	{
		if (Node.FloorLevel == FloorNumber)
		{
			Node.bIsCleared = true;
			break;
		}
	}
}
