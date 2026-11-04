# Wonder Command v3.0 — GitHub Copilot Master Specification & Handoff Guide

Use this document to bring **GitHub Copilot** (in VS Code) completely up to speed with the **Wonder Command v3.0 (Mama & Papa Voice + Multi-Engine + Gov/PDF Hub)** architecture. It aligns directly with the local Python companion and test suite that Copilot already created in your Unreal project directory (`C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon\Companion\`).

## 1. Copy-Paste Master Prompt for GitHub Copilot

Copy the block below and paste it directly into your **GitHub Copilot Chat** in VS Code (or save this file inside `C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon\Companion\COPILOT_V3_SPEC.md` and tell Copilot: *"Read `Companion/COPILOT_V3_SPEC.md` and implement the v3.0 upgrades across `wonder_command_companion.py`, `web/index.html`, and `tests/test_companion.py`"*):

```text
@workspace We are upgrading the local Wonder Command Companion in `C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon\Companion\` to v3.0 so that my phone and PC stay in sync and all v3.0 features work end-to-end while keeping your strict security, LAN token pairing (`web/pair.html`), human approval queue, and zero-PII/zero-SSN rules intact.

Please update `Companion/wonder_command_companion.py`, `Companion/web/index.html` (and sync to `C:\Users\Admin\Music\wonder_command_omni_agent_task_project_system.html`), and `Companion/tests/test_companion.py` to implement these 4 missing modules:

1. Live Mama AI ("Big Mama") & Papa AI ("George Jefferson") Chat + Voice Engine:
   - In `wonder_command_companion.py`, add `POST /api/agent/chat` using `urllib.request` to call the Google Gemini REST API (`https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent`) reading `GEMINI_API_KEY` from `os.environ` (never hardcoded or exposed to the frontend).
   - Support 3 persona modes (`mama`, `papa`, `both`) that return JSON: `{"mama_reply": "...", "papa_reply": "...", "proposed_action": {...}}` and automatically log every interaction into the SQLite database and `.jsonl` log.
   - In `web/index.html`, wire up browser Text-to-Speech (`window.speechSynthesis`) with distinct voice profiles (Mama: warm/steady `pitch: 0.92, rate: 0.95`; Papa: energetic/bold `pitch: 1.25, rate: 1.15`; Co-Op: Mama speaks first, then Papa chimes in) and Web Speech API (`SpeechRecognition` / `webkitSpeechRecognition`) microphone input so I can talk to Mama and Papa from my Android phone or PC.

2. Safe AI-to-App Command Queue (AI Never Touches Raw Files Directly):
   - When Mama or Papa proposes an action in `proposed_action` (such as `CREATE_ENGINE_FOLDERS`, `WRITE_PIPELINE_SCRIPT`, `EXPORT_LEDGER_JSON`, or `STAGE_FILE_COPY`), store it as a `pending_approval` item in SQLite.
   - Only execute the file/script generation inside `C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon\` when I click "Approve & Run" in the UI (`POST /api/actions/approve`).

3. Multi-Engine Game Building Hub Tab (`🏗️ Blender + UE5.8 + Unity + Cities II`):
   - Add a tab in `web/index.html` and helper endpoints in `wonder_command_companion.py` (`POST /api/engine/scaffold` and `POST /api/engine/generate-script`) that safely scaffold sibling folders (`01_Blender_Source`, `02_Shared_Game_Core`, `03_UE58_WonderWorld`, `04_Unity_Brawler_CS2`) and generate:
     a) `sync_wonder_pipeline.py` (validates FBX scale/axes and syncs `WonderLedgerState.json`).
     b) `wonder_blender_ai_addon.py` (Blender 4.x `bpy` sidebar panel for Auto-LOD0/1/2, `UCX_` collision hulls, Smart UV projection, and dual UE5.8/Unity FBX export).
     c) `pack_orm_textures.py` (Python Pillow channel packer combining Ambient Occlusion -> R, Roughness -> G, Metallic -> B into `_ORM.png` for RTX 5060).
     d) `MamaTowerCities2System.cs` (Unity 2022.3 LTS C# DOTS/ECS `GameSystemBase` mod script for Cities: Skylines II with `8m x 8m` grid footprint and `4096x4096` `_ControlMask` / `_Emissive` tower specs).

4. Government Paperwork, PDF Generator & AI File-Format Converter Tab (`📑 Gov Paperwork, PDF & File AI`):
   - Add an interactive form builder in `web/index.html` for: (1) Good Cause Extension Request, (2) SSA-3369-BK Work History Report (15-Year CNA & Caregiver grid), (3) SSA-3373 Function Report Coaching, (4) Landlord-Tenant Orderly Removal Stipulation, and (5) NJ SHARES / Homeless Trust Fund Cover Packet.
   - PRIVACY RULE: Never hardcode SSNs or sensitive PII in HTML or Python. Keep sensitive fields local in memory or masked (`XXX-XX-XXXX`) until the user types them at export time.
   - Include client-side multi-page PDF export (via `jsPDF` or printable PDF view) plus a 4-format AI Schema Converter (`.json`, `.xml`, `.md`, `.csv`) so AI tools can parse case packets cleanly.
```

## 2. Current Workspace Alignment (What Copilot Already Built vs. What v3.0 Adds)

| Component | Existing Path in Your Project | Current Status (Built by Copilot) | v3.0 Upgrade Required |
| :--- | :--- | :--- | :--- |
| **Python Companion Server** | `Companion\wonder_command_companion.py` | Runs on `127.0.0.1:8765` (or `0.0.0.0:8765` with LAN token), SQLite task/log storage, JSONL export, approved file copy/move | Add `/api/agent/chat` (Gemini API proxy with `GEMINI_API_KEY`), `/api/engine/scaffold`, `/api/engine/generate-script`, and `/api/convert/schema` |
| **Main Web UI** | `Companion\web\index.html` | Task categorization (Medical, Social Services, Identity Masking, Game Dev) + companion task queue | Add Mama/Papa TTS & Mic bar, Multi-Engine Game Building tab, and Gov Paperwork + PDF/Schema tab |
| **Phone LAN Pairing** | `Companion\web\pair.html` | Token login page setting `HttpOnly` cookie for Android phone on same Wi-Fi | Preserve as-is; ensure `/api/agent/chat` and voice features work seamlessly after pairing |
| **Automated Tests** | `Companion\tests\test_companion.py` | 8 passing `unittest` tests for tasks, logs, and path traversal safety | Add unit tests for persona chat formatting, pending action approval, schema conversion, and PII redaction |
| **UE5 Blockout Script** | `Scripts\blockout_builder.py` | Builds Wonder Tower blockout inside Unreal Engine | Connect to Multi-Engine Hub so `sync_wonder_pipeline.py` can trigger or validate alongside it |

## 3. Backend Specification (`Companion\wonder_command_companion.py`)

Give Copilot the following backend implementation blueprint to merge into `Companion\wonder_command_companion.py` using only Python's standard library (`urllib.request`, `json`, `os`, `sqlite3`, `pathlib`) so no extra `pip install` is required unless desired.

### 3.1. Mama AI ("Big Mama") & Papa AI ("George Jefferson") System Prompts & Gemini Proxy

```python
import json
import os
import urllib.request
import urllib.error

SYSTEM_PERSONAS = {
    "mama": (
        "You are Mama AI ('Big Mama' energy — warm, protective, no-nonsense matriarch, encouraging, "
        "soulful, and fiercely organized). You call the user 'baby' or 'sugar', keep their spirits up, "
        "break down medical, housing, Social Security, and game-building tasks into calm, manageable steps, "
        "and remind them to protect their health and pace their back pain."
    ),
    "papa": (
        "You are Papa AI ('George Jefferson' energy — sharp, ambitious, fast-talking, executive boss, "
        "focused on upward mobility, 'movin' on up', financial ledger discipline, and shipping the "
        "Wonder World Tower Tycoon & Industrial Brawler empire). You talk strategy, numbers, engine "
        "pipelines, and legal/housing leverage with bold confidence."
    ),
    "both": (
        "You are running in Co-Op Mode with BOTH Mama AI ('Big Mama' — warm, protective, soulful strategist) "
        "and Papa AI ('George Jefferson' — bold, fast-talking executive builder). "
        "Return valid JSON ONLY with keys: "
        '{"mama_reply": "Mama\'s spoken response", "papa_reply": "Papa\'s spoken response", '
        '"proposed_action": null or {"action_type": "CREATE_TASK|SCAFFOLD_ENGINES|GENERATE_SCRIPT", "title": "...", "payload": {}}}'
    )
}

def call_gemini_agent(mode: str, user_message: str, context_summary: str = "") -> dict:
    """Calls Gemini API using GEMINI_API_KEY from environment variables without external dependencies."""
    api_key = os.environ.get("GEMINI_API_KEY", "").strip()
    if not api_key:
        # Safe offline fallback so UI and voice still work before API key is configured
        return {
            "mama_reply": (
                f"Baby, I heard you say: '{user_message}'. My local voice is working right now, "
                "and as soon as you set GEMINI_API_KEY in your terminal environment, I'll have full live AI brains too!"
            ),
            "papa_reply": (
                "That's right! Set `$env:GEMINI_API_KEY='your_key'` in PowerShell before starting "
                "`py Companion\\wonder_command_companion.py`, and we're movin' on up!"
            ),
            "proposed_action": None,
            "live_api": False
        }

    persona_prompt = SYSTEM_PERSONAS.get(mode, SYSTEM_PERSONAS["both"])
    full_prompt = (
        f"{persona_prompt}\n\n"
        "IMPORTANT: Always respond as a valid JSON object with keys: "
        "`mama_reply` (string), `papa_reply` (string), and `proposed_action` (null or object).\n"
        "Never include SSNs or raw credentials.\n"
        f"Project Context: {context_summary}\n"
        f"User Message: {user_message}"
    )

    url = f"https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent?key={api_key}"
    payload = {
        "contents": [{"parts": [{"text": full_prompt}]}],
        "generationConfig": {"responseMimeType": "application/json", "temperature": 0.7}
    }

    req = urllib.request.Request(
        url,
        data=json.dumps(payload).encode("utf-8"),
        headers={"Content-Type": "application/json"},
        method="POST"
    )
    try:
        with urllib.request.urlopen(req, timeout=20) as resp:
            data = json.loads(resp.read().decode("utf-8"))
            raw_text = data["candidates"][0]["content"]["parts"][0]["text"]
            parsed = json.loads(raw_text)
            return {
                "mama_reply": parsed.get("mama_reply", ""),
                "papa_reply": parsed.get("papa_reply", ""),
                "proposed_action": parsed.get("proposed_action"),
                "live_api": True
            }
    except Exception as exc:
        return {
            "mama_reply": "Baby, the network connection to Gemini hiccuped, so I saved your message in the local companion log.",
            "papa_reply": f"Check your connection or API quota ({type(exc).__name__}). Nothing was lost!",
            "proposed_action": None,
            "live_api": False
        }
```

### 3.2. Safe Multi-Engine Workspace Scaffolder & Script Generator (`POST /api/engine/scaffold`)

When approved by the user in the companion app, `wonder_command_companion.py` creates the clean sibling architecture inside a designated subfolder (for example, `WonderPipeline\` inside the project root) so **Blender 4.x**, **Unreal Engine 5.8**, and **Unity 2022.3 LTS** never corrupt each other's `.meta` or `Intermediate` files:

```text
WonderTowerTycoon/
├── Companion/
│   ├── wonder_command_companion.py
│   ├── web/
│   │   ├── index.html
│   │   └── pair.html
│   └── tests/
│       └── test_companion.py
├── Scripts/
│   └── blockout_builder.py
└── WonderPipeline/
    ├── 01_Blender_Source/
    │   ├── Characters_Rigged/
    │   ├── Tower_Modular_Meshes/
    │   └── Scripts/
    │       ├── wonder_blender_ai_addon.py
    │       └── pack_orm_textures.py
    ├── 02_Shared_Game_Core/
    │   ├── State/
    │   │   └── WonderLedgerState.json
    │   ├── Exports_UE58_FBX/
    │   ├── Exports_Unity_FBX/
    │   └── Textures_2K_4K_ORM/
    ├── 03_UE58_WonderWorld/
    │   └── MCP_Bridge_Config.json
    ├── 04_Unity_Brawler_CS2/
    │   └── Mods_CitiesSkylines2/
    │       └── MamaTowerCities2System.cs
    └── sync_wonder_pipeline.py
```

#### Shared 3-Column Financial Ledger (`02_Shared_Game_Core/State/WonderLedgerState.json`)
```json
{
  "schema_version": "3.0.0",
  "tower_name": "Mama Wonder Tower",
  "hardware_profile": "RTX_5060_16GB_RAM",
  "ledger_columns": {
    "column_1_gross_revenue": {
      "arcade_brawler_payouts": 14500.00,
      "commercial_lease_income": 48000.00,
      "cities_skylines_tourism_bonus": 9250.00
    },
    "column_2_operating_expenses": {
      "structural_maintenance": 11200.00,
      "security_and_brawler_staff": 8400.00,
      "utility_grid_load_cost": 5100.00
    },
    "column_3_net_reserve_and_upgrades": {
      "liquid_reserve": 47050.00,
      "unlocked_tier": 3,
      "cities_skylines_lot_size": "8m x 8m",
      "emissive_signage_active": true
    }
  }
}
```

## 4. Frontend Voice, Multi-Engine & Gov/PDF Specification (`Companion\web\index.html`)

### 4.1. Browser Speech Synthesis (`window.speechSynthesis`) & Microphone STT (`webkitSpeechRecognition`)

Copilot should wire these exact voice controls into `Companion\web\index.html` so Mama and Papa speak out loud on both desktop Chrome/Edge and Android Chrome:

```javascript
const VoiceEngine = {
  mode: 'both', // 'mama' | 'papa' | 'both'
  muted: false,
  mamaConfig: { pitch: 0.92, rate: 0.95 }, // Warm, grounded Big Mama cadence
  papaConfig: { pitch: 1.25, rate: 1.15 }, // High-energy George Jefferson cadence

  pickVoice(preferFemale = true) {
    const voices = window.speechSynthesis ? window.speechSynthesis.getVoices() : [];
    if (!voices.length) return null;
    const enVoices = voices.filter(v => /en(-|_)?(US|GB)?/i.test(v.lang));
    const pool = enVoices.length ? enVoices : voices;
    if (preferFemale) {
      return pool.find(v => /Aria|Zira|Jenny|Samantha|Google US English|Female/i.test(v.name)) || pool[0];
    }
    return pool.find(v => /Guy|David|Mark|Christopher|Male/i.test(v.name)) || pool[pool.length - 1];
  },

  speakText(text, speaker = 'mama', onEndCallback = null) {
    if (this.muted || !('speechSynthesis' in window) || !text) {
      if (onEndCallback) onEndCallback();
      return;
    }
    const utter = new SpeechSynthesisUtterance(text);
    const cfg = speaker === 'papa' ? this.papaConfig : this.mamaConfig;
    utter.pitch = cfg.pitch;
    utter.rate = cfg.rate;
    const chosenVoice = this.pickVoice(speaker !== 'papa');
    if (chosenVoice) utter.voice = chosenVoice;
    if (onEndCallback) utter.onend = onEndCallback;
    window.speechSynthesis.speak(utter);
  },

  speakResponse(mamaReply, papaReply) {
    if (!('speechSynthesis' in window)) return;
    window.speechSynthesis.cancel();
    if (this.mode === 'mama' && mamaReply) {
      this.speakText(mamaReply, 'mama');
    } else if (this.mode === 'papa' && papaReply) {
      this.speakText(papaReply, 'papa');
    } else {
      // Co-Op Mode: Mama speaks first, then Papa follows automatically
      this.speakText(mamaReply, 'mama', () => {
        if (papaReply) this.speakText(papaReply, 'papa');
      });
    }
  }
};
```

### 4.2. Multi-Engine Scripts to Embed in the Generator Tab

1. **Blender 4.x AI Builder Add-On (`wonder_blender_ai_addon.py`)**:
   - Registers a `VIEW_3D` UI Sidebar panel (`Wonder Command AI`).
   - **Auto-LOD Operator**: Duplicates selected mesh into `_LOD0` (100%), `_LOD1` (50% Decimate ratio), and `_LOD2` (25% Decimate ratio), plus generates a `UCX_<MeshName>` convex hull for Unreal Engine 5.8 collision.
   - **Dual Engine FBX Exporter**:
     - Exports to `02_Shared_Game_Core/Exports_UE58_FBX/` using `-Y Forward, Z Up`, `apply_unit_scale=True`, `mesh_smooth_type='FACE'`.
     - Exports to `02_Shared_Game_Core/Exports_Unity_FBX/` using `-Z Forward, Y Up`, `apply_scale_options='FBX_SCALE_UNITS'`.

2. **PBR ORM Texture Channel Packer (`pack_orm_textures.py`)**:
   - Uses `PIL.Image` to pack `_AO.png` into Red, `_Roughness.png` into Green, and `_Metallic.png` into Blue, saving a single `_ORM.png` texture in `02_Shared_Game_Core/Textures_2K_4K_ORM/` to cut VRAM consumption on the RTX 5060.

3. **Unity Hub + *Cities: Skylines II* Custom Mod (`MamaTowerCities2System.cs`)**:
   - Uses Unity 2022.3 LTS `Game.Simulation` and `Unity.Entities` (`GameSystemBase`).
   - Reads `WonderLedgerState.json` every simulation tick (`GetUpdateInterval` = 262144 / 16 updates per day), applies the `8m x 8m` zone lot footprint for **Mama Wonder Tower**, and drives the `4096x4096` `_ControlMask` and `_Emissive` window lighting textures based on the 3-column financial reserve.

### 4.3. Government Paperwork, PDF Generator & AI Schema Converter (`📑 Gov Paperwork, PDF & File AI`)

- **Interactive Templates (Zero Hardcoded PII)**:
  1. **Court Good Cause Extension Request**: Pre-fills the legal structure requesting a 14-day extension to submit medical/physician verification while leaving personal identifiers as user-typed fields at export time.
  2. **SSA-3369-BK Work History Report (15-Year CNA & Private Caregiver Grid)**: Pre-populates physical duty descriptions (patient transfers, Hoyer lift operation, 50–100 lb lifting/supporting, charting, ADL assistance, hours standing/walking) so the user can review and export cleanly to PDF.
  3. **SSA-3373 Adult Function Report Guide**: Organizes daily functional limitations (lumbar back flare-ups, lifting limits, rest intervals) in clear clinical language.
  4. **Orderly Removal / Relocation Settlement Worksheet**: Tracks transition timeline dates, deposit handling, and neutral housing reference terms.
  5. **NJ SHARES & Homeless Trust Fund Packet Checklist**: Tracks required enclosures (`social-security-statement.pdf`, lease documentation, utility statement, medical verification).
- **4-Format AI Schema Converter**:
  - Converts any form packet or task list into **JSON** (`.json`), **XML** (`.xml`), **Markdown** (`.md`), or **CSV** (`.csv`) with automatic regex masking for patterns matching `\b\d{3}-\d{2}-\d{4}\b` when "Mask Sensitive Numbers for AI Upload" is checked.

## 5. Unit Tests to Add (`Companion\tests\test_companion.py`)

Ask Copilot to append these 4 unit tests to `Companion\tests\test_companion.py` and run `C:\Users\Admin\AppData\Local\Python\pythoncore-3.14-64\python.exe -m unittest discover -s Companion\tests -p "test_*.py"`:

1. `test_agent_chat_offline_fallback`: Verifies `POST /api/agent/chat` returns both `mama_reply` and `papa_reply` cleanly even when `GEMINI_API_KEY` is unset, and logs the exchange in SQLite.
2. `test_action_approval_gate`: Verifies that an AI-proposed action (`proposed_action`) stays in `pending` state and does not write any files until `POST /api/actions/approve` is called.
3. `test_pii_redaction_filter`: Verifies that the schema converter masks any `###-##-####` SSN pattern as `[REDACTED-SSN]` before exporting `.json`, `.xml`, `.md`, or `.csv` for external AI consumption.
4. `test_engine_scaffold_path_confinement`: Verifies `POST /api/engine/scaffold` only creates `01_Blender_Source`, `02_Shared_Game_Core`, `03_UE58_WonderWorld`, and `04_Unity_Brawler_CS2` inside the allowed project root and rejects `..` path traversal.
