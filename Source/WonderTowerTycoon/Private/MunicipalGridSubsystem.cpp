#include "MunicipalGridSubsystem.h"
#include "MunicipalEconomySubsystem.h"

UMunicipalGridSubsystem::UMunicipalGridSubsystem()
{
}

void UMunicipalGridSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UWorld* World = GetWorld();
	UMunicipalEconomySubsystem* Economy = World ? World->GetSubsystem<UMunicipalEconomySubsystem>() : nullptr;
	ensureMsgf(Economy, TEXT("MunicipalGridSubsystem requires a UMunicipalEconomySubsystem."));
}

bool UMunicipalGridSubsystem::TryPlaceLot(FIntPoint Coordinate, EWonderZone Zone, FString LotId,
	int64 Price, float PowerDemand, float WaterDemand, float Population)
{
	if (!IsCoordinateBuildable(Coordinate))
	{
		return false;
	}
	if (Cells.Contains(Coordinate) || Zone == EWonderZone::Empty || Zone > EWonderZone::Civic || LotId.IsEmpty())
	{
		return false;
	}

	UWorld* World = GetWorld();
	UMunicipalEconomySubsystem* Economy = World ? World->GetSubsystem<UMunicipalEconomySubsystem>() : nullptr;
	if (!Economy)
	{
		return false;
	}
	FString Failure;
	if (!Economy->TrySpendCredits(Price, Failure))
	{
		return false;
	}

	FWonderGridCell Cell;
	Cell.Coordinate = Coordinate;
	Cell.Zone = Zone;
	Cell.LotId = MoveTemp(LotId);
	Cell.IsOccupied = true;
	Cells.Add(Coordinate, Cell);

	const FString SourceId = FString::Printf(TEXT("grid:%s:%d,%d"), *Cell.LotId, Coordinate.X, Coordinate.Y);
	Economy->AddIncomeSource(SourceId, FMath::Max(0.0f, static_cast<float>(Price) * 0.0125f),
		FMath::Max(0.0f, PowerDemand), FMath::Max(0.0f, WaterDemand), FMath::Max(0.0f, Population));
	OnGridChanged.Broadcast(Coordinate, Zone, Cell.LotId);
	return true;
}

bool UMunicipalGridSubsystem::RemoveLot(FIntPoint Coordinate)
{
	FWonderGridCell Removed;
	if (!Cells.RemoveAndCopyValue(Coordinate, Removed))
	{
		return false;
	}
	UWorld* World = GetWorld();
	UMunicipalEconomySubsystem* Economy = World ? World->GetSubsystem<UMunicipalEconomySubsystem>() : nullptr;
	if (Economy)
	{
		Economy->RemoveIncomeSource(FString::Printf(TEXT("grid:%s:%d,%d"), *Removed.LotId, Coordinate.X, Coordinate.Y));
	}
	OnGridChanged.Broadcast(Coordinate, EWonderZone::Empty, TEXT(""));
	return true;
}

bool UMunicipalGridSubsystem::IsCoordinateBuildable(FIntPoint Coordinate) const
{
	return Coordinate.X >= GridMin.X && Coordinate.X <= GridMax.X
		&& Coordinate.Y >= GridMin.Y && Coordinate.Y <= GridMax.Y;
}

FWonderGridCell UMunicipalGridSubsystem::GetCell(FIntPoint Coordinate) const
{
	const FWonderGridCell* Cell = Cells.Find(Coordinate);
	return Cell ? *Cell : FWonderGridCell{};
}

TArray<FWonderGridCell> UMunicipalGridSubsystem::GetOccupiedCells() const
{
	TArray<FWonderGridCell> Result;
	Cells.GenerateValueArray(Result);
	return Result;
}
