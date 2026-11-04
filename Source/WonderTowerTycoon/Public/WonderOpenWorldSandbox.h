#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"
#include "WonderOpenWorldSandbox.generated.h"

class UStaticMeshComponent;
class UFloatingPawnMovement;
class UWonderMasterControlComponent;

UENUM(BlueprintType)
enum class EWonderTraversalMode : uint8
{
    ThirdPersonOnFoot UMETA(DisplayName = "Third-Person Street Walk"),
    StreetDriving     UMETA(DisplayName = "Street Cruiser Driving"),
    FlyingCar         UMETA(DisplayName = "Purple City Flying Car")
};

USTRUCT(BlueprintType)
struct FWonderCityBuildingSpec
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|CityBuilder")
    FName BuildingType = TEXT("CommercialSoundTower");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|CityBuilder")
    FIntPoint GridCell = FIntPoint(0, 0);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|CityBuilder")
    int32 Floors = 4;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|CityBuilder")
    float ConstructionCost = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|CityBuilder")
    float IncomePerSecond = 120.0f;
};

/**
 * AWonderHoverVehiclePawn
 * Hybrid Street-Driving Sports Cruiser & Vertical-Takeoff Flying Car for Unreal Engine 5.8.
 */
UCLASS(Blueprintable)
class WONDERTOWERTYCOON_API AWonderHoverVehiclePawn : public APawn
{
    GENERATED_BODY()

public:
    AWonderHoverVehiclePawn();

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WonderWorld|Vehicle")
    TObjectPtr<UStaticMeshComponent> VehicleBodyMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WonderWorld|Vehicle")
    TObjectPtr<UWonderMasterControlComponent> MasterControlComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|Vehicle")
    EWonderTraversalMode TraversalMode = EWonderTraversalMode::StreetDriving;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|Vehicle")
    float MaxStreetSpeed = 2400.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|Vehicle")
    float MaxFlightSpeed = 3800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|Vehicle")
    float VerticalAscentRate = 1200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|Vehicle")
    float TurnRateDegreesPerSec = 110.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WonderWorld|Vehicle")
    float CurrentForwardSpeed = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WonderWorld|Vehicle")
    bool bSonicAfterburnerActive = false;

    UFUNCTION(BlueprintCallable, Category = "WonderWorld|Vehicle")
    void ToggleFlyingCarMode();

    UFUNCTION(BlueprintCallable, Category = "WonderWorld|Vehicle")
    void ApplyThrottleInput(float AxisValue, float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "WonderWorld|Vehicle")
    void ApplySteeringInput(float AxisValue, float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "WonderWorld|Vehicle")
    void ApplyVerticalFlightInput(float AxisValue, float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "WonderWorld|Vehicle")
    void SetSonicAfterburner(bool bEnabled);
};

/**
 * AWonderOpenWorldSandboxManager
 * Open-World Sandbox City Builder & Traffic Director connecting UMunicipalGridSubsystem
 * and UWonderLedgerSubsystem inside Unreal Engine 5.8.
 */
UCLASS(Blueprintable)
class WONDERTOWERTYCOON_API AWonderOpenWorldSandboxManager : public AActor
{
    GENERATED_BODY()

public:
    AWonderOpenWorldSandboxManager();

    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WonderWorld|CityBuilder")
    TArray<FWonderCityBuildingSpec> PlacedBuildings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|CityBuilder")
    int32 MamaTowerFloors = 6;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|CityBuilder")
    int32 PapaTowerFloors = 6;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|CityBuilder")
    int32 ActiveFlyingCarsInSky = 12;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WonderWorld|CityBuilder")
    float IncomeCommitInterval = 1.0f;

    UFUNCTION(BlueprintCallable, Category = "WonderWorld|CityBuilder")
    bool PlaceSandboxBuilding(FIntPoint GridCell, FName BuildingType, int32 Floors, float Cost, float IncomePerSec);

    UFUNCTION(BlueprintCallable, Category = "WonderWorld|CityBuilder")
    bool UpgradeHeroTwinTowers(float UpgradeCost = 2000.0f);

    UFUNCTION(BlueprintCallable, Category = "WonderWorld|CityBuilder")
    float CalculateTotalCityIncomePerSecond() const;

private:
    float AccumulatedCityYield = 0.0f;
    float TimeSinceLastIncomeCommit = 0.0f;
};
