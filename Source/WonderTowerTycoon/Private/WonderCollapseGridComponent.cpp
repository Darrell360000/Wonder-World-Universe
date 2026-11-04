#include "WonderCollapseGridComponent.h"

UWonderCollapseGridComponent::UWonderCollapseGridComponent()
	: Rows(5)
	, Columns(5)
	, TileSpacing(200.0f)
	, DefaultWarningTime(2.0f)
	, AutoRestoreDelay(8.0f)
	, bAutoRestoreTiles(true)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UWonderCollapseGridComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (FWonderGridTile& Tile : Tiles)
	{
		if (Tile.State == ETileCollapseState::WarningWobble)
		{
			Tile.StateTimer -= DeltaTime;
			if (Tile.StateTimer <= 0.0f)
			{
				Tile.State = ETileCollapseState::Collapsed;
				Tile.StateTimer = AutoRestoreDelay;
				OnTileStateChanged.Broadcast(Tile.TileIndex, ETileCollapseState::Collapsed);
			}
		}
		else if (Tile.State == ETileCollapseState::Collapsed && bAutoRestoreTiles)
		{
			Tile.StateTimer -= DeltaTime;
			if (Tile.StateTimer <= 0.0f)
			{
				Tile.State = ETileCollapseState::Solid;
				Tile.StateTimer = 0.0f;
				OnTileStateChanged.Broadcast(Tile.TileIndex, ETileCollapseState::Solid);
			}
		}
	}
}

void UWonderCollapseGridComponent::InitializeGrid(int32 InRows, int32 InCols, float InSpacing)
{
	Rows = FMath::Max(1, InRows);
	Columns = FMath::Max(1, InCols);
	TileSpacing = InSpacing;
	Tiles.Empty(Rows * Columns);

	const float OriginX = -(Rows - 1) * TileSpacing * 0.5f;
	const float OriginY = -(Columns - 1) * TileSpacing * 0.5f;

	int32 Index = 0;
	for (int32 r = 0; r < Rows; ++r)
	{
		for (int32 c = 0; c < Columns; ++c)
		{
			FWonderGridTile NewTile;
			NewTile.TileIndex = Index++;
			NewTile.RelativeLocation = FVector(OriginX + r * TileSpacing, OriginY + c * TileSpacing, 0.0f);
			NewTile.State = ETileCollapseState::Solid;
			NewTile.StateTimer = 0.0f;
			Tiles.Add(NewTile);
		}
	}
}

void UWonderCollapseGridComponent::TriggerTileCollapse(int32 TileIndex, float WarningTime)
{
	if (Tiles.IsValidIndex(TileIndex) && Tiles[TileIndex].State == ETileCollapseState::Solid)
	{
		Tiles[TileIndex].State = ETileCollapseState::WarningWobble;
		Tiles[TileIndex].StateTimer = WarningTime > 0.0f ? WarningTime : DefaultWarningTime;
		OnTileStateChanged.Broadcast(TileIndex, ETileCollapseState::WarningWobble);
	}
}

void UWonderCollapseGridComponent::StepOnTile(int32 TileIndex)
{
	TriggerTileCollapse(TileIndex, DefaultWarningTime);
}

void UWonderCollapseGridComponent::RestoreTile(int32 TileIndex)
{
	if (Tiles.IsValidIndex(TileIndex) && Tiles[TileIndex].State != ETileCollapseState::Solid)
	{
		Tiles[TileIndex].State = ETileCollapseState::Solid;
		Tiles[TileIndex].StateTimer = 0.0f;
		OnTileStateChanged.Broadcast(TileIndex, ETileCollapseState::Solid);
	}
}

void UWonderCollapseGridComponent::RestoreAllTiles()
{
	for (FWonderGridTile& Tile : Tiles)
	{
		if (Tile.State != ETileCollapseState::Solid)
		{
			Tile.State = ETileCollapseState::Solid;
			Tile.StateTimer = 0.0f;
			OnTileStateChanged.Broadcast(Tile.TileIndex, ETileCollapseState::Solid);
		}
	}
}

int32 UWonderCollapseGridComponent::GetSolidTileCount() const
{
	int32 Count = 0;
	for (const FWonderGridTile& Tile : Tiles)
	{
		if (Tile.State == ETileCollapseState::Solid)
		{
			Count++;
		}
	}
	return Count;
}

bool UWonderCollapseGridComponent::IsTileSolid(int32 TileIndex) const
{
	return Tiles.IsValidIndex(TileIndex) && Tiles[TileIndex].State == ETileCollapseState::Solid;
}
