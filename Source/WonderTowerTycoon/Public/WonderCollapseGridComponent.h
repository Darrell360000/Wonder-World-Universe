#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderCollapseGridComponent.generated.h"

UENUM(BlueprintType)
enum class ETileCollapseState : uint8
{
	Solid           UMETA(DisplayName = "Solid"),
	WarningWobble   UMETA(DisplayName = "Warning Wobble"),
	Collapsed       UMETA(DisplayName = "Collapsed"),
	Restoring       UMETA(DisplayName = "Restoring")
};

USTRUCT(BlueprintType)
struct FWonderGridTile
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
	int32 TileIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
	FVector RelativeLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
	ETileCollapseState State = ETileCollapseState::Solid;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
	float StateTimer = 0.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGridTileStateChanged, int32, TileIndex, ETileCollapseState, NewState);

/**
 * Manages procedural crumbling floor tiles for boss arenas, Crumbling Blocks party trials,
 * and God Hand spatial puzzles.
 */
UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERTOWERTYCOON_API UWonderCollapseGridComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWonderCollapseGridComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Config", meta = (ClampMin = "1", ClampMax = "50"))
	int32 Rows;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Config", meta = (ClampMin = "1", ClampMax = "50"))
	int32 Columns;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Config", meta = (ClampMin = "50.0"))
	float TileSpacing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Config")
	float DefaultWarningTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Config")
	float AutoRestoreDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Config")
	bool bAutoRestoreTiles;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid State")
	TArray<FWonderGridTile> Tiles;

	UPROPERTY(BlueprintAssignable, Category = "Grid Events")
	FOnGridTileStateChanged OnTileStateChanged;

	UFUNCTION(BlueprintCallable, Category = "Wonder|Grid")
	void InitializeGrid(int32 InRows, int32 InCols, float InSpacing = 200.0f);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Grid")
	void TriggerTileCollapse(int32 TileIndex, float WarningTime = 1.5f);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Grid")
	void StepOnTile(int32 TileIndex);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Grid")
	void RestoreTile(int32 TileIndex);

	UFUNCTION(BlueprintCallable, Category = "Wonder|Grid")
	void RestoreAllTiles();

	UFUNCTION(BlueprintPure, Category = "Wonder|Grid")
	int32 GetSolidTileCount() const;

	UFUNCTION(BlueprintPure, Category = "Wonder|Grid")
	bool IsTileSolid(int32 TileIndex) const;
};
