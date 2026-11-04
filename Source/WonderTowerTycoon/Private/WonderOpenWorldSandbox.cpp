#include "WonderOpenWorldSandbox.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "MunicipalGridSubsystem.h"
#include "WonderLedgerSubsystem.h"
#include "WonderMasterControlComponent.h"
#include "Components/InputComponent.h"

AWonderHoverVehiclePawn::AWonderHoverVehiclePawn()
{
    PrimaryActorTick.bCanEverTick = true;

    VehicleBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VehicleBodyMesh"));
    SetRootComponent(VehicleBodyMesh);

    MasterControlComponent = CreateDefaultSubobject<UWonderMasterControlComponent>(TEXT("MasterControlComponent"));
    if (MasterControlComponent)
    {
        MasterControlComponent->ControlMode = EWonderControlMode::InVehicle;
    }
}

void AWonderHoverVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (MasterControlComponent)
    {
        MasterControlComponent->SetupControlBindings(PlayerInputComponent);
    }
}

void AWonderHoverVehiclePawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!FMath::IsNearlyZero(CurrentForwardSpeed))
    {
        const FVector DeltaMove = GetActorForwardVector() * CurrentForwardSpeed * DeltaTime;
        AddActorWorldOffset(DeltaMove, true);
        CurrentForwardSpeed = FMath::FInterpTo(CurrentForwardSpeed, 0.0f, DeltaTime, 1.5f);
    }
}

void AWonderHoverVehiclePawn::ToggleFlyingCarMode()
{
    if (TraversalMode == EWonderTraversalMode::FlyingCar)
    {
        TraversalMode = EWonderTraversalMode::StreetDriving;
    }
    else
    {
        TraversalMode = EWonderTraversalMode::FlyingCar;
        AddActorWorldOffset(FVector(0.0f, 0.0f, 180.0f), false);
    }
}

void AWonderHoverVehiclePawn::ApplyThrottleInput(float AxisValue, float DeltaTime)
{
    const float MaxSpeed = (TraversalMode == EWonderTraversalMode::FlyingCar) ? MaxFlightSpeed : MaxStreetSpeed;
    const float BoostMult = bSonicAfterburnerActive ? 1.65f : 1.0f;
    const float TargetSpeed = FMath::Clamp(AxisValue, -1.0f, 1.0f) * MaxSpeed * BoostMult;
    CurrentForwardSpeed = FMath::FInterpTo(CurrentForwardSpeed, TargetSpeed, DeltaTime, 4.0f);
}

void AWonderHoverVehiclePawn::ApplySteeringInput(float AxisValue, float DeltaTime)
{
    if (FMath::IsNearlyZero(AxisValue))
    {
        return;
    }
    const float YawDelta = AxisValue * TurnRateDegreesPerSec * DeltaTime;
    const float RollTarget = (TraversalMode == EWonderTraversalMode::FlyingCar) ? (AxisValue * 22.0f) : 0.0f;
    FRotator CurrentRot = GetActorRotation();
    CurrentRot.Yaw += YawDelta;
    CurrentRot.Roll = FMath::FInterpTo(CurrentRot.Roll, RollTarget, DeltaTime, 6.0f);
    SetActorRotation(CurrentRot);
}

void AWonderHoverVehiclePawn::ApplyVerticalFlightInput(float AxisValue, float DeltaTime)
{
    if (TraversalMode != EWonderTraversalMode::FlyingCar || FMath::IsNearlyZero(AxisValue))
    {
        return;
    }
    const float ZDelta = FMath::Clamp(AxisValue, -1.0f, 1.0f) * VerticalAscentRate * DeltaTime;
    FVector NewLoc = GetActorLocation();
    NewLoc.Z = FMath::Max(60.0f, NewLoc.Z + ZDelta);
    SetActorLocation(NewLoc, true);
}

void AWonderHoverVehiclePawn::SetSonicAfterburner(bool bEnabled)
{
    bSonicAfterburnerActive = bEnabled;
}

AWonderOpenWorldSandboxManager::AWonderOpenWorldSandboxManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AWonderOpenWorldSandboxManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (DeltaTime <= 0.0f || !FMath::IsFinite(DeltaTime))
    {
        return;
    }

    const float IncomeRate = CalculateTotalCityIncomePerSecond();
    if (IncomeRate > 0.0f && FMath::IsFinite(IncomeRate))
    {
        AccumulatedCityYield += IncomeRate * DeltaTime;
    }

    TimeSinceLastIncomeCommit += DeltaTime;
    const float CommitThreshold = FMath::Max(0.25f, IncomeCommitInterval);
    if (TimeSinceLastIncomeCommit < CommitThreshold || AccumulatedCityYield <= 0.0f)
    {
        return;
    }

    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GI = World->GetGameInstance())
        {
            if (UWonderLedgerSubsystem* Ledger = GI->GetSubsystem<UWonderLedgerSubsystem>())
            {
                const float YieldToCommit = AccumulatedCityYield;
                AccumulatedCityYield = 0.0f;
                TimeSinceLastIncomeCommit = 0.0f;
                Ledger->RecordGrindIncome(YieldToCommit, 0, TEXT("OpenWorldCityBuilder"));
            }
        }
    }
}

bool AWonderOpenWorldSandboxManager::PlaceSandboxBuilding(FIntPoint GridCell, FName BuildingType, int32 Floors, float Cost, float IncomePerSec)
{
    if (BuildingType.IsNone() || Floors < 1 || !FMath::IsFinite(Cost) || Cost <= 0.0f || !FMath::IsFinite(IncomePerSec) || IncomePerSec < 0.0f)
    {
        return false;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }

    UMunicipalGridSubsystem* Grid = World->GetSubsystem<UMunicipalGridSubsystem>();
    UGameInstance* GI = World->GetGameInstance();
    UWonderLedgerSubsystem* Ledger = GI ? GI->GetSubsystem<UWonderLedgerSubsystem>() : nullptr;
    if (!Grid || !Ledger)
    {
        return false;
    }

    if (!Grid->IsCoordinateBuildable(GridCell) || Grid->GetCell(GridCell).IsOccupied)
    {
        return false;
    }

    Ledger->AuditAndBalanceLedger();
    if (Ledger->Column3Foundation.LiquidReserve < Cost)
    {
        return false;
    }

    const int64 LotPrice = FMath::Max<int64>(1, FMath::RoundToInt64(Cost));
    if (!Grid->TryPlaceLot(GridCell, EWonderZone::Commercial, BuildingType.ToString(), LotPrice, 10.0f, 5.0f, 25.0f))
    {
        return false;
    }

    Ledger->Column2Overhead.StructuralMaintenance += Cost;
    Ledger->AuditAndBalanceLedger();
    Ledger->SaveSharedLedgerJson();

    FWonderCityBuildingSpec Spec;
    Spec.GridCell = GridCell;
    Spec.BuildingType = BuildingType;
    Spec.Floors = Floors;
    Spec.ConstructionCost = Cost;
    Spec.IncomePerSecond = IncomePerSec;
    PlacedBuildings.Add(Spec);

    if (BuildingType == TEXT("SkyPortFlyingGarage"))
    {
        ActiveFlyingCarsInSky += 4;
    }

    return true;
}

bool AWonderOpenWorldSandboxManager::UpgradeHeroTwinTowers(float UpgradeCost)
{
    if (!FMath::IsFinite(UpgradeCost) || UpgradeCost <= 0.0f)
    {
        return false;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }

    UGameInstance* GI = World->GetGameInstance();
    UWonderLedgerSubsystem* Ledger = GI ? GI->GetSubsystem<UWonderLedgerSubsystem>() : nullptr;
    if (!Ledger)
    {
        return false;
    }

    Ledger->AuditAndBalanceLedger();
    if (Ledger->Column3Foundation.LiquidReserve < UpgradeCost)
    {
        return false;
    }

    Ledger->Column2Overhead.StructuralMaintenance += UpgradeCost;
    Ledger->AuditAndBalanceLedger();
    Ledger->SaveSharedLedgerJson();

    MamaTowerFloors++;
    PapaTowerFloors++;
    return true;
}

float AWonderOpenWorldSandboxManager::CalculateTotalCityIncomePerSecond() const
{
    float Total = (MamaTowerFloors + PapaTowerFloors) * 25.0f;
    for (const FWonderCityBuildingSpec& Spec : PlacedBuildings)
    {
        Total += Spec.IncomePerSecond;
    }
    return Total;
}
