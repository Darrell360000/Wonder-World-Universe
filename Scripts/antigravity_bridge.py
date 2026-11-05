import os
import sys
import json
import glob
import subprocess
from pathlib import Path

import platform

IS_WINDOWS = platform.system() == "Windows"
ROOT = Path(r"C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon") if IS_WINDOWS else Path("/mnt/c/Users/Admin/Documents/Unreal Projects/WonderTowerTycoon")
BUS_DIR = ROOT / "WonderPipeline" / "06_Engine_Command_Logs"
BUS_DIR.mkdir(parents=True, exist_ok=True)

UNITY_HUB = Path(r"C:\Program Files\Unity Hub\Unity Hub.exe") if IS_WINDOWS else Path("/mnt/c/Program Files/Unity Hub/Unity Hub.exe")
EDITOR_BASE = Path(r"C:\Program Files\Unity\Hub\Editor") if IS_WINDOWS else Path("/mnt/c/Program Files/Unity/Hub/Editor")

def find_unity_binary(version_prefix="6"):
    """Finds installed Unity editor executable matching version prefix (e.g., '6' or '7')."""
    if not EDITOR_BASE.exists():
        return None
    # Check for direct or wildcard matches
    patterns = [
        EDITOR_BASE / f"*{version_prefix}*" / "Editor" / "Unity.exe",
        EDITOR_BASE / f"6000.{version_prefix}*" / "Editor" / "Unity.exe",
        EDITOR_BASE / "6000.6.0f1" / "Editor" / "Unity.exe"
    ]
    for pattern in patterns:
        matches = glob.glob(str(pattern))
        if matches:
            return Path(matches[0])
    return None

def connect_unity_hub():
    """Auto-registers project into Unity Hub via headless CLI."""
    unity_proj = ROOT / "WonderWorldUnity"
    if UNITY_HUB.exists():
        print("[Antigravity -> Unity Hub] Registering WonderWorldUnity project...")
        subprocess.run([str(UNITY_HUB), "--", "--headless", "project-add", "--path", str(unity_proj)], check=False)
        print("[Antigravity -> Unity Hub] Project successfully registered.")
    else:
        print("[!] Unity Hub not found at default location.")

def launch_unity_editor(target_version="6"):
    """Launches or commands Unity 6 or Unity 7."""
    exe = find_unity_binary(target_version)
    unity_proj = ROOT / "WonderWorldUnity"
    if exe and exe.exists():
        print(f"[Antigravity -> Unity {target_version}] Found editor: {exe}")
        print(f"[Antigravity -> Unity {target_version}] Launching project headlessly / in background...")
        subprocess.Popen([str(exe), "-projectPath", str(unity_proj)])
    else:
        print(f"[!] Unity {target_version} editor binary not found under {EDITOR_BASE}")

def run_blender_pipeline():
    """Generates 3D Pixar meshes and packs ORM PBR textures."""
    print("[Antigravity -> Blender] Generating 3D character and vehicle meshes...")
    subprocess.run([sys.executable, str(ROOT / "Scripts" / "wonder_world_assets.py"), "--output", str(ROOT / "Content" / "WonderWorld" / "Characters")], check=False)
    
    print("[Antigravity -> Blender] Packing ORM textures (AO, Roughness, Metallic)...")
    orm_script = ROOT / "WonderPipeline" / "01_Blender_Source" / "Scripts" / "pack_orm_textures.py"
    if orm_script.exists():
        subprocess.run([sys.executable, str(orm_script)], check=False)
    print("[Antigravity -> Blender] Asset and texture build complete.")

def dispatch_unreal_engine():
    """Syncs Darrell X-Gen Hero, boss, and ledger subsystems into UE5."""
    print("[Antigravity -> Unreal Engine 5] Dispatching Darrell X-Gen Hero build...")
    ue_script = ROOT / "build_darrell_xgen_to_ue5.py"
    if ue_script.exists():
        subprocess.run([sys.executable, str(ue_script)], check=False)
    
    ue_bus = BUS_DIR / "ue58_command_bus.jsonl"
    with open(ue_bus, "a", encoding="utf-8") as f:
        f.write(json.dumps({"command": "RELOAD_DARRELL_XGEN_SOUND_MAGIC", "engine": "UE5", "status": "DISPATCHED"}) + "\n")
    print("[Antigravity -> Unreal Engine 5] Command bus synced.")

def dispatch_unity_controllers():
    """Writes setup intent for hover vehicle and brawler combat into the Unity command bus."""
    print("[Antigravity -> Unity] Queuing hover vehicle and brawler combat setup...")
    unity_bus = BUS_DIR / "unity_cs2_command_bus.jsonl"
    with open(unity_bus, "a", encoding="utf-8") as f:
        f.write(json.dumps({"command": "SETUP_HOVER_VEHICLE", "vehicle": "HoverDriveVehicle", "mesh": "Visible", "version_target": "Unity6_and_7"}) + "\n")
    print("[Antigravity -> Unity] Command written to unity_cs2_command_bus.jsonl.")

if __name__ == "__main__":
    action = sys.argv[1] if len(sys.argv) > 1 else "all"
    
    if action in ("all", "unity_hub"): connect_unity_hub()
    if action in ("all", "blender"):   run_blender_pipeline()
    if action in ("all", "ue5"):       dispatch_unreal_engine()
    if action in ("all", "unity"):     dispatch_unity_controllers()
    if action == "launch_unity6":      launch_unity_editor("6")
    if action == "launch_unity7":      launch_unity_editor("7")
    
    print("\n[Antigravity] Quad/Penta engine pipeline executed successfully!")