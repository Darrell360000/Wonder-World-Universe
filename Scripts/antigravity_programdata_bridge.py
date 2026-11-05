import os
import sys
import json
import platform
import subprocess
from pathlib import Path

IS_WINDOWS = platform.system() == "Windows"
WORKSPACE_DIR = (
    Path(r"C:\Users\Admin\.gemini\antigravity\scratch\Wonder-World-Universe")
    if IS_WINDOWS
    else Path("/mnt/c/Users/Admin/.gemini/antigravity/scratch/Wonder-World-Universe")
)
MANIFEST_PATH = WORKSPACE_DIR / "antigravity_full_system_bridge.json"


def to_posix_path(win_path: str) -> str:
    """Helper to convert a Windows drive path to POSIX under WSL/Linux."""
    if not win_path:
        return win_path
    if not IS_WINDOWS:
        if win_path.startswith("C:") or win_path.startswith("c:"):
            return "/mnt/c" + win_path[2:].replace("\\", "/")
        if win_path.startswith("D:") or win_path.startswith("d:"):
            return "/mnt/d" + win_path[2:].replace("\\", "/")
        if win_path.startswith("E:") or win_path.startswith("e:"):
            return "/mnt/e" + win_path[2:].replace("\\", "/")
        if win_path.startswith("F:") or win_path.startswith("f:"):
            return "/mnt/f" + win_path[2:].replace("\\", "/")
        if win_path.startswith("L:") or win_path.startswith("l:"):
            return "/mnt/l" + win_path[2:].replace("\\", "/")
    return win_path


def open_item(target_path: str):
    """Cross-platform launcher for folders and files."""
    if hasattr(os, "startfile"):
        os.startfile(target_path)
    else:
        posix = to_posix_path(target_path)
        print(f"[HOST/WSL] Path resolved: {posix}")


class AntigravitySystemBridge:
    def __init__(self, manifest_path: Path = MANIFEST_PATH):
        self.manifest_path = manifest_path
        self.data = self._load_manifest()

    def _load_manifest(self) -> dict:
        if not self.manifest_path.exists():
            print(f"[BRIDGE] Manifest not found at {self.manifest_path}. Running PowerShell scanner first...")
            ps_script = WORKSPACE_DIR / "Sync-AllProgramDataToAntigravity.ps1"
            powershell_bin = (
                r"C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe"
                if IS_WINDOWS
                else "/mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe"
            )
            subprocess.run([
                powershell_bin,
                "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(ps_script)
            ], check=False)

        with open(self.manifest_path, "r", encoding="utf-8-sig") as f:
            return json.load(f)

    def launch_office_app(self, app_name: str, file_to_open: str = None):
        """Launch any Microsoft Office 16 application (Word, Excel, OneNote, PowerPoint, Outlook, Access, Publisher)."""
        office_map = self.data.get("Office16Executables", {})
        lookup = {
            "word": "WINWORD.EXE",
            "excel": "EXCEL.EXE",
            "powerpoint": "POWERPNT.EXE",
            "outlook": "OUTLOOK.EXE",
            "onenote": "ONENOTE.EXE",
            "access": "MSACCESS.EXE",
            "publisher": "MSPUB.EXE",
            "skype": "lync.exe",
            "orgchart": "ORGCHART.EXE"
        }
        exe_key = lookup.get(app_name.lower(), app_name)
        exe_path = office_map.get(exe_key) or rf"C:\Program Files\Microsoft Office\root\Office16\{exe_key}"
        check_path = exe_path if IS_WINDOWS else to_posix_path(exe_path)

        if os.path.exists(check_path):
            cmd = [exe_path]
            if file_to_open:
                cmd.append(file_to_open)
            print(f"[OFFICE 16] Launching: {' '.join(cmd)}")
            if IS_WINDOWS:
                subprocess.Popen(cmd)
            else:
                print(f"[CONFIRMED] Office binary located at {check_path}")
        else:
            print(f"[ERROR] Office executable not found: {exe_path}")

    def launch_start_menu_item(self, query: str):
        """Search and launch any of the 88 Start Menu ProgramData folders, subfolders, or resolved .lnk targets."""
        query_lower = query.lower()

        # 1. Check Explicit 88 Map first
        explicit_map = self.data.get("Explicit88StartMenuMap", {})
        for name, path in explicit_map.items():
            if query_lower == name.lower():
                print(f"[START MENU 88] Opening exact match: {name} -> {path}")
                open_item(path)
                return True

        # 2. Search Recursive Start Menu Tree (subfolders & .lnk shortcuts)
        tree = self.data.get("RecursiveStartMenuTree", [])
        for item in tree:
            name = item.get("Name", "")
            if query_lower in name.lower():
                target_exe = item.get("TargetExe")
                full_path = item.get("FullPath")
                check_exe = target_exe if IS_WINDOWS else to_posix_path(target_exe)
                check_full = full_path if IS_WINDOWS else to_posix_path(full_path)
                if check_exe and os.path.exists(check_exe):
                    print(f"[RESOLVED SHORTCUT] Launching {name} -> {target_exe}")
                    if IS_WINDOWS:
                        subprocess.Popen([target_exe])
                    else:
                        print(f"[CONFIRMED] Shortcut target binary: {check_exe}")
                    return True
                elif check_full and os.path.exists(check_full):
                    print(f"[START MENU ITEM] Opening {name} -> {full_path}")
                    open_item(full_path)
                    return True

        print(f"[WARN] No matching Start Menu or ProgramData item found for: '{query}'")
        return False

    def list_summary(self):
        """Print a quick summary of all bridged directories and tools."""
        explicit_count = len(self.data.get("Explicit88StartMenuMap", {}))
        office_count = len(self.data.get("Office16Executables", {}))
        program_data_count = len(self.data.get("ProgramDataDirectories", []))
        recursive_count = len(self.data.get("RecursiveStartMenuTree", []))

        print("=" * 70)
        print("🌌 ANTIGRAVITY FULL SYSTEM BRIDGE SUMMARY")
        print("=" * 70)
        print(f" • Top-Level Start Menu Items Mapped : {explicit_count}")
        print(f" • Recursive Start Menu Sub-Items    : {recursive_count}")
        print(f" • Microsoft Office 16 Executables   : {office_count}")
        print(f" • C:\\ProgramData Root Directories   : {program_data_count}")
        print("=" * 70)


if __name__ == "__main__":
    bridge = AntigravitySystemBridge()
    if len(sys.argv) < 2:
        bridge.list_summary()
        print("\nUsage Examples:")
        print("  python antigravity_programdata_bridge.py office onenote")
        print("  python antigravity_programdata_bridge.py office excel")
        print("  python antigravity_programdata_bridge.py launch \"Unity 6000.6.0f1\"")
        print("  python antigravity_programdata_bridge.py launch \"Studio One 7\"")
        print("  python antigravity_programdata_bridge.py launch \"Maxon\"")
    else:
        command = sys.argv[1].lower()
        target = sys.argv[2] if len(sys.argv) > 2 else ""
        if command == "office" and target:
            bridge.launch_office_app(target)
        elif command == "launch" and target:
            bridge.launch_start_menu_item(target)
        else:
            bridge.list_summary()
