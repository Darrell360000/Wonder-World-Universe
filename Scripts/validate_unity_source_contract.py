"""Validate the standalone Unity play-slice source contract in hosted CI."""

from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SOURCE_ROOT = ROOT / "WonderWorldUnity" / "Assets" / "Scripts"
RUNTIME_ROOT = SOURCE_ROOT / "Runtime"


def validate() -> None:
    required = {
        "WonderCameraDirector.cs": ("WonderCameraDirector", "SwitchMode", "WonderCameraMode", "StopCoroutine", "FinishTransition"),
        "BrawlerCombatController.cs": ("BrawlerCombatController", "TryAttack", "ReceiveDamage", "IDamageable", "HashSet<IDamageable>", "float.IsNaN(amount)"),
        "EscortMissionController.cs": ("EscortMissionController", "StartMission", "ReportEscortDamage", "EscortMissionState", "initialEscortDistances", "Progress = measuredProgress"),
        "HeroManager.cs": ("HeroManager", "HotSwapToHero", "TopHatterHeroSlot", "OnHeroHotSwapped"),
        "WonderLedgerAndGrindController.cs": ("WonderLedgerAndGrindController", "PumpWetVac", "DumpWetVacTank", "PerformCNATask", "TakeMamaRestBreak", "RecalculateLedger"),
        "MamaTowerCities2System.cs": ("MamaTowerCities2System", "SimulationTicksPerDay", "SyncFromSharedLedgerJson", "8m x 8m"),
    }

    for filename, symbols in required.items():
        path = RUNTIME_ROOT / filename
        source = path.read_text(encoding="utf-8")
        missing = [symbol for symbol in symbols if symbol not in source]
        if missing:
            raise AssertionError(f"{filename} missing Unity source contract: {', '.join(missing)}")

    tests = SOURCE_ROOT / "Tests" / "EscortMissionControllerTests.cs"
    if not tests.is_file():
        raise AssertionError("Escort mission EditMode tests are missing")
    camera_tests = SOURCE_ROOT / "Tests" / "WonderCameraDirectorTests.cs"
    brawler_tests = SOURCE_ROOT / "Tests" / "BrawlerCombatControllerTests.cs"
    if not camera_tests.is_file():
        raise AssertionError("Camera director EditMode tests are missing")
    if not brawler_tests.is_file():
        raise AssertionError("Brawler combat EditMode tests are missing")

    print("VALIDATED Wonder World Unity & Cities: Skylines II source contracts")


if __name__ == "__main__":
    validate()
