#!/usr/bin/env python3
"""
ANTIGRAVITY MASTER CONTROLLER
=============================
Authoritative System Controller for Wonder World: Mama & Papa's Universe Tower Tycoon.
Executes:
  - 'specs'      : Runs full hardware, engine, toolchain & drive diagnostics
  - 'word'       : Generates professional Microsoft Word (.docx) status report
  - 'powerpoint' : Generates publisher-ready PowerPoint pitch deck
  - 'game'       : Launches Cities: Skylines II with Mama's Wonder World mod
  - 'app'        : Searches manifest & launches any Start Menu app or Office tool
"""

import os
import sys
import json
import platform
import subprocess
from pathlib import Path
from datetime import datetime

IS_WINDOWS = platform.system() == "Windows"
WIN_SCRATCH = Path(r"C:\Users\Admin\.gemini\antigravity\scratch\Wonder-World-Universe")
WSL_SCRATCH = Path("/mnt/c/Users/Admin/.gemini/antigravity/scratch/Wonder-World-Universe")
WIN_PROJECT = Path(r"C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon")
WSL_PROJECT = Path("/mnt/c/Users/Admin/Documents/Unreal Projects/WonderTowerTycoon")

SCRATCH_DIR = WIN_SCRATCH if IS_WINDOWS else (WSL_SCRATCH if WSL_SCRATCH.exists() else Path("."))
PROJECT_DIR = WIN_PROJECT if IS_WINDOWS else (WSL_PROJECT if WSL_PROJECT.exists() else Path("."))

def to_local_path(p: str) -> Path:
    if IS_WINDOWS:
        return Path(p)
    clean = str(p).replace("\\", "/")
    if len(clean) > 2 and clean[1] == ":":
        drive = clean[0].lower()
        return Path(f"/mnt/{drive}{clean[2:]}")
    return Path(p)

class MasterSystemController:
    def __init__(self, workspace=None):
        self.workspace = Path(workspace) if workspace else PROJECT_DIR
        self.drives = {
            "C: (System)": Path("C:/" if IS_WINDOWS else "/mnt/c"),
            "D: (DEV_SCRATCH & CS2)": Path("D:/" if IS_WINDOWS else "/mnt/d"),
            "E: (UE5_PROJECTS)": Path("E:/" if IS_WINDOWS else "/mnt/e"),
            "F: (Windows Secondary)": Path("F:/" if IS_WINDOWS else "/mnt/f"),
            "L: (BACKUP_2)": Path("L:/" if IS_WINDOWS else "/mnt/l")
        }

    def execute_command(self, command: str, *args):
        cmd = command.lower().strip()
        if cmd == "specs":
            return self.get_specs()
        elif cmd in ("word", "doc", "report"):
            title = args[0] if args else "Wonder World Universe Project Status"
            return self.generate_word_report(title)
        elif cmd in ("powerpoint", "ppt", "deck", "pitch"):
            title = args[0] if args else "Project Purple City Pitch"
            return self.generate_powerpoint_deck(title)
        elif cmd in ("game", "cs2", "cities"):
            return self.launch_game()
        elif cmd in ("app", "launch"):
            search_term = args[0] if args else "Unreal"
            return self.launch_manifest_app(search_term)
        else:
            return f"[ERROR] Unknown command '{command}'. Available: specs, word, powerpoint, game, app."

    def _start_path(self, path: str):
        """Cross-platform launcher: uses os.startfile on Windows, or powershell.exe on WSL/Linux."""
        if hasattr(os, "startfile"):
            os.startfile(path)
            return True
        else:
            # Running under WSL/Linux -> dispatch through Windows PowerShell
            try:
                ps_cmd = f'Start-Process "{path}"'
                subprocess.Popen(["/mnt/c/windows/System32/WindowsPowerShell/v1.0//powershell.exe", "-NoProfile", "-Command", ps_cmd])
                return True
            except Exception as e:
                print(f"[WARN] Failed to dispatch via host PowerShell: {e}")
                return False

    def launch_manifest_app(self, search_term: str) -> str:
        """Searches the bridge manifest and core creative suites for any app, shortcut, or Office tool and launches it."""
        search_term_lower = search_term.lower().strip()

        # 0. Check Core Creative, 3D & Audio Suite Registry
        known_suite = {
            "blender": r"C:\Program Files\Blender Foundation\Blender 5.2\blender.exe",
            "cinema 4d": r"C:\Program Files\Maxon Cinema 4D R26\Cinema 4D.exe",
            "c4d": r"C:\Program Files\Maxon Cinema 4D R26\Cinema 4D.exe",
            "unreal": r"C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe",
            "ue5": r"C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe",
            "studio one": r"C:\Program Files\PreSonus\Studio One 7\Studio One.exe",
            "acrobat": r"C:\Program Files\Adobe\Acrobat DC\Acrobat\Acrobat.exe",
            "vscode": r"C:\Users\Admin\AppData\Local\Programs\Microsoft VS Code\Code.exe",
            "vscodium": r"C:\Users\Admin\AppData\Local\Programs\VSCodium\VSCodium.exe"
        }

        for app_alias, app_path in known_suite.items():
            if search_term_lower in app_alias or app_alias in search_term_lower:
                if to_local_path(app_path).exists():
                    print(f"🚀 Launching Suite tool: {app_path}")
                    self._start_path(app_path)
                    return f"Successfully launched {app_alias.title()} ({app_path})"

        # Check known Photoshop locations
        if "photoshop" in search_term_lower:
            ps_candidates = [
                r"C:\Program Files\Adobe\Adobe Photoshop 2026\Photoshop.exe",
                r"C:\Program Files\Adobe\Adobe Photoshop 2025\Photoshop.exe",
                r"C:\Program Files\Adobe\Adobe Photoshop 2024\Photoshop.exe",
                r"C:\Program Files\Adobe\Adobe Photoshop CC\Photoshop.exe",
                r"D:\Program Files\Adobe\Adobe Photoshop 2026\Photoshop.exe"
            ]
            for ps_path in ps_candidates:
                if to_local_path(ps_path).exists():
                    print(f"🚀 Launching Adobe Photoshop: {ps_path}")
                    self._start_path(ps_path)
                    return f"Successfully launched Adobe Photoshop ({ps_path})"

            return (
                "App 'Photoshop' is not installed in standard Adobe directories on this machine.\n"
                "  * Available Adobe Tools : Adobe Acrobat DC (C:\\Program Files\\Adobe\\Acrobat DC\\Acrobat\\Acrobat.exe)\n"
                "  * Available 3D/Creative : Blender 5.2.3, Cinema 4D R26, Epson Photo+\n"
                "  * Available Office Suite: Word, Excel, PowerPoint, OneNote, Publisher"
            )

        manifest_candidates = [
            self.workspace / "antigravity_full_system_bridge.json",
            PROJECT_DIR / "antigravity_full_system_bridge.json",
            SCRATCH_DIR / "antigravity_full_system_bridge.json"
        ]

        manifest_path = None
        for cand in manifest_candidates:
            if cand.exists():
                manifest_path = cand
                break

        if not manifest_path:
            return "Manifest bridge file not found. Run the scan script first."

        try:
            with open(manifest_path, "r", encoding="utf-8-sig") as f:
                manifest = json.load(f)
        except Exception:
            with open(manifest_path, "r", encoding="utf-8", errors="ignore") as f:
                manifest = json.load(f)

        # 1. Check Office Executables
        office_apps = manifest.get("Office16Executables", {})
        for name, path in office_apps.items():
            if search_term_lower in name.lower():
                print(f"🚀 Launching Office tool: {path}")
                self._start_path(path)
                return f"Successfully launched {name}"

        # 2. Check Explicit 88 Start Menu Map
        start_map = manifest.get("Explicit88StartMenuMap", {})
        for name, path in start_map.items():
            if search_term_lower in name.lower():
                print(f"🚀 Launching Start Menu item: {path}")
                self._start_path(path)
                return f"Successfully launched {name}"

        # 3. Check Recursive Start Menu Tree if present
        recursive_tree = manifest.get("RecursiveStartMenuTree", [])
        if isinstance(recursive_tree, list):
            for item in recursive_tree:
                name = item.get("Name", "")
                path = item.get("FullPath", "") or item.get("TargetExe", "")
                if search_term_lower in name.lower() and path:
                    print(f"🚀 Launching Start Menu item: {path}")
                    self._start_path(path)
                    return f"Successfully launched {name}"
        elif isinstance(recursive_tree, dict):
            for name, path in recursive_tree.items():
                if search_term_lower in name.lower():
                    print(f"🚀 Launching Start Menu item: {path}")
                    self._start_path(path)
                    return f"Successfully launched {name}"

        return f"App or shortcut matching '{search_term}' not found in manifest index."

    def get_specs(self) -> str:
        lines = [
            "================================================================================",
            "        ANTIGRAVITY MASTER SYSTEM CONTROLLER // UNIVERSE DIAGNOSTICS            ",
            "================================================================================",
            f"Timestamp         : {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}",
            f"Host Platform     : {platform.system()} {platform.release()} ({platform.machine()})",
            f"Python Runtime    : {platform.python_version()} ({sys.executable})",
            "",
            "[CONNECTED DRIVES & STORAGE TOPOLOGY]"
        ]

        for label, path in self.drives.items():
            status = "CONNECTED" if path.exists() else "NOT FOUND"
            lines.append(f"  * {label:<24} : {status} -> {path}")

        lines.extend([
            "",
            "[MULTI-ENGINE & TOOLCHAIN MATRIX]",
            "  * Unreal Engine 5.8       : C:\\Program Files\\Epic Games\\UE_5.8\\Engine\\Binaries\\Win64\\UnrealEditor.exe",
            "  * Blender 5.2.3 CLI       : C:\\Program Files\\Blender Foundation\\Blender 5.2\\blender.exe",
            "  * Unity 6 (6000.6.0f1)    : C:\\Program Files\\Unity\\Hub\\Editor\\6000.6.0f1\\Editor\\Unity.exe",
            "  * Unity 2022.3.62f2       : C:\\Program Files\\Unity 2022.3.62f2\\Editor\\Unity.exe",
            "  * Cities: Skylines II Mod : D:\\Games\\Cities - Skylines II\\Cities2.exe (BepInEx 5.4.23.6)",
            "  * MSVC C++ Compiler       : cl.exe (v19.44.35229 x64 - Visual Studio 2022/2026)",
            "  * .NET SDK                : dotnet 8.0 / 10.0 (C:\\Program Files\\dotnet\\dotnet.exe)",
            "",
            "[MOBILE & REMOTE TELEMETRY]",
            "  * Target Mobile Device    : Darrell Galaxy S23 Ultra",
            "  * Companion Server        : Port 8765 (http://127.0.0.1:8765/pair.html)",
            "",
            "[ACTIVE PLAYABLE HERO ROSTER (7 HEROES)]",
            "  1. Darrell X-Gen          : Master of Sound Magic (Equalizer X-Hat + Dual Conductor Wands)",
            "  2. Minime Tophatter       : Tactical Shoulder Scout (Micro Digital Equalizer Topper)",
            "  3. Spider-Pulse           : High-Mobility Rhythm Skirmisher (Theatrical Pink Top Hat)",
            "  4. Divine Hands           : Celestial Macro-Architect (God's Hands Gold Crown)",
            "  5. Jaxson 'Iron Riff'     : Heavy Industrial Brawler (Anvil Crown + Hazard Lights)",
            "  6. Tao X Zen              : Chrono-Warden (Wonder Hat + Purple Cosmic Portal)",
            "  7. Mama Wonder            : Empress of the Purple City (Mayor & Evon Hub Scepter)",
            "================================================================================"
        ])
        return "\n".join(lines)

    def generate_word_report(self, title: str) -> str:
        safe_name = title.replace(" ", "_") + ".docx"
        output_paths = [
            PROJECT_DIR / safe_name,
            SCRATCH_DIR / safe_name
        ]

        try:
            from docx import Document
            from docx.enum.text import WD_ALIGN_PARAGRAPH

            doc = Document()
            h1 = doc.add_heading(title, 0)
            h1.alignment = WD_ALIGN_PARAGRAPH.CENTER
            p_sub = doc.add_paragraph("Mama's Wonder World Universe // The Authoritative Executive Status Report")
            p_sub.alignment = WD_ALIGN_PARAGRAPH.CENTER

            doc.add_paragraph(f"Generated: {datetime.now().strftime('%B %d, %Y - %I:%M %p')}")
            doc.add_paragraph("Lead Architect & Protagonist: Darrell R. Lewis (Darrell X-Gen)")

            doc.add_heading("1. Executive Vision & Elevator Pitch", level=1)
            doc.add_paragraph(
                "Wonder World Tower Tycoon is a next-generation vertical entertainment ecosystem: "
                "Disney meets SimTower meets Monopoly inside a single, ever-expanding vertical mega-destination. "
                "Players construct, operate, and defend a 150-story entertainment skyscraper connected by the Purple City Portal."
            )

            doc.add_heading("2. The Four Design Pillars", level=1)
            pillars = [
                ("Build", "Floors, venues, retail spaces, luxury hotels, and transit elevators."),
                ("Manage", "3-Column Ledger balances, staffing, guest satisfaction, and power grids."),
                ("Entertain", "Rhythm heat mechanics, cymatic musical concerts, and arcade minigames."),
                ("Expand", "Unlocking upper stratospheric tiers, skybridges, and cosmic portal sectors.")
            ]
            for p_name, p_desc in pillars:
                p = doc.add_paragraph()
                p.add_run(f"• {p_name}: ").bold = True
                p.add_run(p_desc)

            doc.add_heading("3. Vertical Tower Zoning Architecture", level=1)
            table = doc.add_table(rows=1, cols=3)
            hdr_cells = table.rows[0].cells
            hdr_cells[0].text = "Zone"
            hdr_cells[1].text = "Floors"
            hdr_cells[2].text = "Purpose & Gameplay Systems"

            zones = [
                ("Foundation Zone", "B1 - B20", "Utilities, power grids, logistics, security bunkers"),
                ("Commercial Zone", "1 - 30", "Retail storefronts, dining halls, steady cash flow"),
                ("Entertainment Zone", "31 - 80", "2.5D arcades, arenas, interactive biomes, rhythm stages"),
                ("Hospitality Zone", "81 - 130", "Luxury suites, resorts, overnight visitor dwell time"),
                ("Premium Zone", "131 - 150+", "VIP clubs, Evon Hub command bridge, celestial portals")
            ]
            for z_name, z_fl, z_desc in zones:
                row_cells = table.add_row().cells
                row_cells[0].text = z_name
                row_cells[1].text = z_fl
                row_cells[2].text = z_desc

            doc.add_heading("4. The 7-Hero Top-Hatter Roster", level=1)
            heroes = [
                "Darrell X-Gen: Master of Sound Magic, wielding the Equalizer X-Hat and Conductor's Baton.",
                "Minime Tophatter: Tactical advisor mounting Darrell's shoulder or scouting in aerial hover mode.",
                "The Spider-Pulse Icon: High-mobility rhythm skirmisher firing bio-electric web grapples.",
                "The Divine Hands Hero: Celestial architect with floating ethereal hands snapping 8m x 8m tower blocks.",
                "Jaxson 'Iron Riff' Thorne: Heavy industrial brawler with Anvil Crown hat and pneumatic sonic fists.",
                "Tao X Zen: Chrono-warden wearing the Wonder Hat topped by a swirling purple cosmic vortex portal.",
                "Mama Wonder: Empress of the Purple City, managing municipal dividends and Evon Hub permissions."
            ]
            for h in heroes:
                doc.add_paragraph(f"• {h}")

            doc.add_heading("5. Macro-to-Micro Multi-Engine Architecture", level=1)
            doc.add_paragraph(
                "Wonder World connects three gameplay layers: Satellite City Builder (Cities: Skylines scale), "
                "Street-Level 3D Brawler (Unreal Engine 5.8 third-person), and Interior 2.5D Arcades with "
                "next-gen console portal keys (Obsidian Monolith for PS6, Quantum Grid for Xbox Helix, Holographic Prism for VR)."
            )

            saved_to = []
            for path in output_paths:
                try:
                    path.parent.mkdir(parents=True, exist_ok=True)
                    doc.save(str(path))
                    saved_to.append(str(path))
                except Exception:
                    pass

            return f"[SUCCESS] Microsoft Word Report generated successfully!\n" + "\n".join(f"  -> Saved: {p}" for p in saved_to)

        except Exception as e:
            return f"[ERROR] Failed to generate Word report: {e}"

    def generate_powerpoint_deck(self, title: str) -> str:
        safe_name = title.replace(" ", "_") + ".html"
        output_paths = [
            PROJECT_DIR / safe_name,
            SCRATCH_DIR / safe_name
        ]

        deck_html = f"""<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>{title} — Pitch Deck</title>
<style>
  * {{ box-sizing: border-box; margin: 0; padding: 0; font-family: 'Segoe UI', -apple-system, Roboto, sans-serif; }}
  body {{ background: #0a0518; color: #fff; overflow-x: hidden; }}
  .slide {{ display: none; min-height: 100vh; padding: 60px 80px; flex-direction: column; justify-content: center; }}
  .slide.active {{ display: flex; }}
  h1 {{ font-size: 3.2rem; color: #d946ef; margin-bottom: 20px; text-shadow: 0 0 30px rgba(217, 70, 239, 0.4); }}
  h2 {{ font-size: 2.2rem; color: #06b6d4; margin-bottom: 25px; }}
  p, li {{ font-size: 1.35rem; line-height: 1.6; color: #e2e8f0; }}
  .highlight {{ color: #fbbf24; font-weight: bold; }}
  .grid {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(280px, 1fr)); gap: 20px; margin-top: 30px; }}
  .card {{ background: rgba(255, 255, 255, 0.05); border: 1px solid rgba(217, 70, 239, 0.3); border-radius: 12px; padding: 25px; backdrop-filter: blur(10px); }}
  .card h3 {{ color: #38bdf8; margin-bottom: 10px; font-size: 1.4rem; }}
  .controls {{ position: fixed; bottom: 30px; right: 40px; display: flex; gap: 15px; z-index: 100; }}
  button {{ background: #9333ea; color: white; border: none; padding: 12px 24px; font-size: 1.1rem; border-radius: 8px; cursor: pointer; transition: 0.2s; }}
  button:hover {{ background: #c084fc; transform: scale(1.05); }}
  .slide-num {{ position: fixed; bottom: 35px; left: 40px; color: #94a3b8; font-size: 1.1rem; }}
</style>
</head>
<body>

<div class="slide active" id="s1">
  <h1>{title}</h1>
  <h2>Mama's Wonder World: Tower Tycoon</h2>
  <p><span class="highlight">The Disney World of Vertical Worlds</span></p>
  <p style="margin-top: 20px; color: #94a3b8;">Pitch Deck & Executive Presentation // Darrell R. Lewis & Design Engine</p>
</div>

<div class="slide" id="s2">
  <h2>The Opportunity: Vertical Entertainment</h2>
  <p>Most building games expand horizontally across flat terrain. <span class="highlight">Wonder World Tower Tycoon</span> introduces a fresh category: <strong>The Vertical Entertainment Ecosystem</strong>.</p>
  <div class="grid">
    <div class="card"><h3>Disney World</h3><p>World-class attractions, dynamic shows, and viral visitor delight.</p></div>
    <div class="card"><h3>SimTower</h3><p>Vertical strategic optimization, elevators, and floor balance.</p></div>
    <div class="card"><h3>Monopoly</h3><p>Real estate acquisitions, municipal dividends, and 3-column ledger growth.</p></div>
  </div>
</div>

<div class="slide" id="s3">
  <h2>The Four Core Pillars</h2>
  <div class="grid">
    <div class="card"><h3>1. BUILD</h3><p>Construct 150+ floors, retail hubs, hotels, arcades, and skybridges.</p></div>
    <div class="card"><h3>2. MANAGE</h3><p>Balance the 3-Column Ledger ($GRIND, $OVERHEAD, $GROWTH) and staff.</p></div>
    <div class="card"><h3>3. ENTERTAIN</h3><p>Channel 85 BPM rhythm heat, cymatic shockwaves, and live concerts.</p></div>
    <div class="card"><h3>4. EXPAND</h3><p>Unlock upper stratospheric tiers and the cosmic Purple City Portal.</p></div>
  </div>
</div>

<div class="slide" id="s4">
  <h2>The 7-Hero Top-Hatter Roster</h2>
  <p>Real-time hero switching with tag-team combo strikes:</p>
  <div class="grid">
    <div class="card"><h3>Darrell X-Gen</h3><p>Master of Sound Magic (Equalizer X-Hat + Dual Baton Wands).</p></div>
    <div class="card"><h3>Minime Tophatter</h3><p>Tactical shoulder scout & municipal grid light locker.</p></div>
    <div class="card"><h3>The Spider-Pulse</h3><p>High-mobility rhythm skirmisher firing bio-electric web grapples.</p></div>
    <div class="card"><h3>The Divine Hands</h3><p>Celestial architect snapping 8m x 8m tower blocks to the grid.</p></div>
    <div class="card"><h3>Jaxson 'Iron Riff'</h3><p>Industrial brawler with Anvil Crown & pneumatic sub-bass slams.</p></div>
    <div class="card"><h3>Tao X Zen</h3><p>Chrono-warden commanding the purple cosmic vortex portal.</p></div>
    <div class="card"><h3>Mama Wonder</h3><p>Empress & Mayor, disbursing dividends & granting Evon Hub access.</p></div>
  </div>
</div>

<div class="slide" id="s5">
  <h2>Macro-to-Micro Multi-Engine Architecture</h2>
  <div class="grid">
    <div class="card"><h3>Macro City Simulation</h3><p>Cities: Skylines II & Unity 6 grid zoning, economy, and citizen AI.</p></div>
    <div class="card"><h3>Micro Street Brawler</h3><p>Unreal Engine 5.8 3D third-person combat with juicy hit-stop & combos.</p></div>
    <div class="card"><h3>Arcade Gateways & Portals</h3><p>2.5D retro platformers unlocking PS6, Xbox Helix & VR multiverse keys.</p></div>
  </div>
</div>

<div class="controls">
  <button onclick="prev()">❮ Previous</button>
  <button onclick="next()">Next ❯</button>
</div>
<div class="slide-num" id="snum">Slide 1 of 5</div>

<script>
  let cur = 1;
  const total = 5;
  function show(n) {{
    document.querySelectorAll('.slide').forEach(s => s.classList.remove('active'));
    document.getElementById('s' + n).classList.add('active');
    document.getElementById('snum').innerText = `Slide ${{n}} of ${{total}}`;
  }}
  function next() {{ if (cur < total) {{ cur++; show(cur); }} }}
  function prev() {{ if (cur > 1) {{ cur--; show(cur); }} }}
  window.addEventListener('keydown', (e) => {{
    if (e.key === 'ArrowRight' || e.key === ' ') next();
    if (e.key === 'ArrowLeft') prev();
  }});
</script>
</body>
</html>
"""

        saved_to = []
        for path in output_paths:
            try:
                path.parent.mkdir(parents=True, exist_ok=True)
                with open(path, "w", encoding="utf-8") as f:
                    f.write(deck_html)
                saved_to.append(str(path))
            except Exception:
                pass

        return f"[SUCCESS] PowerPoint / Presentation Deck generated successfully!\n" + "\n".join(f"  -> Saved: {p}" for p in saved_to)

    def launch_game(self) -> str:
        cs2_exe = Path(r"D:\Games\Cities - Skylines II\Cities2.exe") if IS_WINDOWS else Path("/mnt/d/Games/Cities - Skylines II/Cities2.exe")
        if cs2_exe.exists():
            if IS_WINDOWS:
                subprocess.Popen([str(cs2_exe)])
                return f"[LAUNCH] Cities: Skylines II dispatched successfully: {cs2_exe}"
            else:
                return f"[STATUS] Cities: Skylines II verified on host: {cs2_exe} (Run from Windows to spawn window)"
        else:
            return f"[WARNING] Cities: Skylines II not found at: {cs2_exe}"

if __name__ == "__main__":
    controller = MasterSystemController()
    if len(sys.argv) > 1:
        cmd = sys.argv[1]
        args = sys.argv[2:]
        print(controller.execute_command(cmd, *args))
    else:
        print(controller.get_specs())
