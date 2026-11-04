"""
Synchronizes the 4-Folder Separated-to-Unified Pipeline (WonderPipeline/):
- Verifies 01_Blender_Source, 02_Shared_Game_Core, 03_UE58_WonderWorld, 04_Unity_Brawler_CS2, and 06_Engine_Command_Logs
- Audits 02_Shared_Game_Core/State/WonderLedgerState.json (3-Column Financial Ledger & Evon Hub Permission Gate)
- Copies shared C++/C# contracts and ORM textures across the multi-engine folders
"""

import json
import os
import shutil

PIPELINE_ROOT = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.dirname(PIPELINE_ROOT)

REQUIRED_DIRS = [
    "01_Blender_Source/Characters_Rigged",
    "01_Blender_Source/Tower_Modular_Meshes",
    "01_Blender_Source/Scripts",
    "02_Shared_Game_Core/State",
    "02_Shared_Game_Core/Exports_UE58_FBX",
    "02_Shared_Game_Core/Exports_Unity_FBX",
    "02_Shared_Game_Core/Textures_2K_4K_ORM",
    "03_UE58_WonderWorld/Source/WonderWorld",
    "04_Unity_Brawler_CS2/Assets/WonderShared",
    "06_Engine_Command_Logs",
]


def main():
    print("=== WonderPipeline Multi-Engine Synchronizer ===")
    for rel_dir in REQUIRED_DIRS:
        full_dir = os.path.join(PIPELINE_ROOT, rel_dir)
        os.makedirs(full_dir, exist_ok=True)
        print(f"[OK] Directory ready: {rel_dir}")

    # Verify WonderLedgerState.json
    ledger_path = os.path.join(PIPELINE_ROOT, "02_Shared_Game_Core", "State", "WonderLedgerState.json")
    with open(ledger_path, "r", encoding="utf-8") as f:
        state = json.load(f)

    col1 = sum(state["ledger_columns"]["column_1_gross_revenue"].values())
    col2 = sum(state["ledger_columns"]["column_2_operating_expenses"].values())
    col3 = state["ledger_columns"]["column_3_net_reserve_and_upgrades"]["liquid_reserve"]
    print(f"[LEDGER] Tower: {state['tower_name']} | Era {state['current_era']} ({state.get('era_name', 'Concrete Grind')})")
    print(f"         Col 1 Gross: ${col1:,.2f} | Col 2 Overhead: ${col2:,.2f} | Col 3 Liquid Reserve: ${col3:,.2f}")
    print(f"         Evon Hub Permission Granted: {state['evon_hub_permission_granted']}")

    # Sync UE5.8 C++ headers/sources into 03_UE58_WonderWorld/Source/WonderWorld for reference
    ue_src_public = os.path.join(PROJECT_ROOT, "Source", "WonderTowerTycoon", "Public")
    ue_src_private = os.path.join(PROJECT_ROOT, "Source", "WonderTowerTycoon", "Private")
    ue_dest = os.path.join(PIPELINE_ROOT, "03_UE58_WonderWorld", "Source", "WonderWorld")

    for fname in ("WonderLedgerSubsystem.h", "WonderGrindMiniGameComponent.h", "DarrellXGenCharacter.h", "SyndicateOverlordBoss.h"):
        src = os.path.join(ue_src_public, fname)
        if os.path.isfile(src):
            shutil.copy2(src, os.path.join(ue_dest, fname))
    for fname in ("WonderLedgerSubsystem.cpp", "WonderGrindMiniGameComponent.cpp", "DarrellXGenCharacter.cpp", "SyndicateOverlordBoss.cpp"):
        src = os.path.join(ue_src_private, fname)
        if os.path.isfile(src):
            shutil.copy2(src, os.path.join(ue_dest, fname))

    # Sync Unity & Cities: Skylines II scripts into 04_Unity_Brawler_CS2/Assets/WonderShared
    unity_src = os.path.join(PROJECT_ROOT, "WonderWorldUnity", "Assets", "Scripts", "Runtime")
    unity_dest = os.path.join(PIPELINE_ROOT, "04_Unity_Brawler_CS2", "Assets", "WonderShared")
    for fname in ("HeroManager.cs", "WonderLedgerAndGrindController.cs", "MamaTowerCities2System.cs"):
        src = os.path.join(unity_src, fname)
        if os.path.isfile(src):
            shutil.copy2(src, os.path.join(unity_dest, fname))

    # Ensure command bus logs exist
    for bus in ("ue58_command_bus.jsonl", "unity_cs2_command_bus.jsonl"):
        bus_path = os.path.join(PIPELINE_ROOT, "06_Engine_Command_Logs", bus)
        if not os.path.isfile(bus_path):
            with open(bus_path, "w", encoding="utf-8") as bf:
                bf.write(json.dumps({"event": "pipeline_initialized", "schema": "3.0.0"}) + "\n")

    print("=== WonderPipeline Sync Complete! ===")


if __name__ == "__main__":
    main()
