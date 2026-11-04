"""Validate municipal & Wonder Ledger Unreal subsystems in both editor and standalone CI runs."""

from pathlib import Path
import uuid

ROOT = Path(__file__).resolve().parents[1]
SOURCE_ROOT = ROOT / "Source" / "WonderTowerTycoon"


def validate_source_contract() -> None:
    economy_header = (SOURCE_ROOT / "Public" / "MunicipalEconomySubsystem.h").read_text(encoding="utf-8")
    economy_source = (SOURCE_ROOT / "Private" / "MunicipalEconomySubsystem.cpp").read_text(encoding="utf-8")
    grid_header = (SOURCE_ROOT / "Public" / "MunicipalGridSubsystem.h").read_text(encoding="utf-8")
    grid_source = (SOURCE_ROOT / "Private" / "MunicipalGridSubsystem.cpp").read_text(encoding="utf-8")
    ledger_header = (SOURCE_ROOT / "Public" / "WonderLedgerSubsystem.h").read_text(encoding="utf-8")
    ledger_source = (SOURCE_ROOT / "Private" / "WonderLedgerSubsystem.cpp").read_text(encoding="utf-8")
    grind_header = (SOURCE_ROOT / "Public" / "WonderGrindMiniGameComponent.h").read_text(encoding="utf-8")
    darrell_header = (SOURCE_ROOT / "Public" / "DarrellXGenCharacter.h").read_text(encoding="utf-8")
    boss_header = (SOURCE_ROOT / "Public" / "SyndicateOverlordBoss.h").read_text(encoding="utf-8")

    required_contract = (
        (economy_header, "class WONDERTOWERTYCOON_API UMunicipalEconomySubsystem", "economy subsystem declaration"),
        (economy_header, "TrySpendCredits", "credit transaction API"),
        (economy_header, "ActivateNetPowerOvercharge", "Net-Power overcharge API"),
        (economy_header, "GetSnapshot", "economy snapshot API"),
        (economy_source, "UMunicipalEconomySubsystem::ActivateNetPowerOvercharge", "overcharge implementation"),
        (economy_source, "std::modf", "fractional credit carry"),
        (economy_source, "BoostedIntervalCount", "overcharge expiry payout handling"),
        (economy_source, "Pair.Value.WaterDemand", "water demand accounting"),
        (grid_header, "class WONDERTOWERTYCOON_API UMunicipalGridSubsystem", "grid subsystem declaration"),
        (grid_header, "TryPlaceLot", "grid placement API"),
        (grid_header, "IsCoordinateBuildable", "grid validation API"),
        (grid_source, "UMunicipalGridSubsystem::TryPlaceLot", "grid placement implementation"),
        (grid_source, "Zone > EWonderZone::Civic", "invalid zoning rejection"),
        (ledger_header, "class WONDERTOWERTYCOON_API UWonderLedgerSubsystem", "3-Column Ledger subsystem"),
        (ledger_header, "RegisterCoronationHit", "specialist coronation API"),
        (ledger_source, "UWonderLedgerSubsystem::SaveSharedLedgerJson", "shared ledger JSON sync"),
        (grind_header, "class WONDERTOWERTYCOON_API UWonderGrindMiniGameComponent", "Wet-Vac & CNA mini-game component"),
        (darrell_header, "class WONDERTOWERTYCOON_API ADarrellXGenCharacter", "Darrell X-Gen character class"),
        (boss_header, "class WONDERTOWERTYCOON_API ASyndicateOverlordBoss", "3-Phase Syndicate Overlord boss class"),
    )

    for content, needle, description in required_contract:
        if not content or needle not in content:
            raise AssertionError(f"Municipal source contract missing: {description}")


def validate_editor_runtime() -> None:
    import unreal

    world = unreal.EditorLevelLibrary.get_editor_world()
    economy = world.get_subsystem(unreal.MunicipalEconomySubsystem)
    grid = world.get_subsystem(unreal.MunicipalGridSubsystem)
    assert economy is not None, "Municipal economy subsystem unavailable"
    assert grid is not None, "Municipal grid subsystem unavailable"
    assert grid.is_coordinate_buildable(unreal.IntPoint(0, 0)), "Origin must be buildable"
    assert not grid.is_coordinate_buildable(unreal.IntPoint(99, 99)), "Out-of-range coordinate accepted"

    zone_enum = getattr(unreal, "EWonderZone", None) or getattr(unreal, "WonderZone", None)
    assert zone_enum is not None, "WonderZone enum binding unavailable"
    occupied = {
        (cell.coordinate.x, cell.coordinate.y)
        for cell in grid.get_occupied_cells()
    }
    coordinate = next(
        (unreal.IntPoint(x, y) for x in range(-32, 32) for y in range(-32, 32)
         if (x, y) not in occupied and grid.is_coordinate_buildable(unreal.IntPoint(x, y))),
        None,
    )
    assert coordinate is not None, "No empty buildable grid coordinate available for validation"

    lot_placed = False
    try:
        lot_placed = grid.try_place_lot(
            coordinate, zone_enum.Residential, "Validation_" + uuid.uuid4().hex, 0, 50.0, 10.0, 12.0
        )
        assert lot_placed, "Grid placement validation failed"
        cell = grid.get_cell(coordinate)
        assert cell.is_occupied, "Placed lot was not recorded in the grid"
        assert grid.remove_lot(coordinate), "Lot removal failed"
        lot_placed = False

        spend_result = economy.try_spend_credits(100000)
        purchase_accepted = spend_result[0] if isinstance(spend_result, tuple) else spend_result
        assert purchase_accepted is False, "Unaffordable purchase accepted"
        assert economy.get_snapshot() is not None, "Economy snapshot unavailable"
    finally:
        if lot_placed:
            grid.remove_lot(coordinate)

    unreal.log("VALIDATED municipal economy/grid subsystem API")


def main() -> None:
    validate_source_contract()
    try:
        validate_editor_runtime()
    except ModuleNotFoundError as error:
        if error.name != "unreal":
            raise
        print("VALIDATED municipal economy/grid & 3-Column Wonder Ledger source contracts (editor runtime requires Unreal)")
    else:
        print("VALIDATED municipal economy/grid source contract and runtime API")


if __name__ == "__main__":
    main()
