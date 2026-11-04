#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MunicipalEconomySubsystem.h"
#include "MunicipalGridSubsystem.generated.h"

UENUM(BlueprintType)
enum class EWonderZone : uint8
{
	Empty,
	Residential,
	Commercial,
	Industrial,
	Civic
};

USTRUCT(BlueprintType)
struct WONDERTOWERTYCOON_API FWonderGridCell
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) FIntPoint Coordinate = FIntPoint::ZeroValue;
	UPROPERTY(BlueprintReadOnly) EWonderZone Zone = EWonderZone::Empty;
	UPROPERTY(BlueprintReadOnly) FString LotId;
	UPROPERTY(BlueprintReadOnly) bool IsOccupied = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FWonderGridChanged, FIntPoint, Coordinate, EWonderZone, Zone, const FString&, LotId);

UCLASS()
class WONDERTOWERTYCOON_API UMunicipalGridSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UMunicipalGridSubsystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category="Wonder World|Grid")
	bool TryPlaceLot(FIntPoint Coordinate, EWonderZone Zone, FString LotId, int64 Price,
		float PowerDemand, float WaterDemand, float Population);
	UFUNCTION(BlueprintCallable, Category="Wonder World|Grid")
	bool RemoveLot(FIntPoint Coordinate);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Wonder World|Grid")
	bool IsCoordinateBuildable(FIntPoint Coordinate) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Wonder World|Grid")
	FWonderGridCell GetCell(FIntPoint Coordinate) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Wonder World|Grid")
	TArray<FWonderGridCell> GetOccupiedCells() const;
	UPROPERTY(BlueprintAssignable, Category="Wonder World|Grid") FWonderGridChanged OnGridChanged;

private:
	UPROPERTY() TMap<FIntPoint, FWonderGridCell> Cells;
	UPROPERTY(EditDefaultsOnly, Category="Wonder World|Grid") FIntPoint GridMin = FIntPoint(-32, -32);
	UPROPERTY(EditDefaultsOnly, Category="Wonder World|Grid") FIntPoint GridMax = FIntPoint(31, 31);
};
