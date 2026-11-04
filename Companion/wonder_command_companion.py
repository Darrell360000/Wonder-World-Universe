"""Local-first task, activity, and approved file-operation bridge for Wonder Command."""

from __future__ import annotations

import argparse
import csv
from contextlib import contextmanager
import hashlib
import hmac
import io
import json
import os
import re
import secrets
import shutil
import sqlite3
import sys
import threading
import uuid
from datetime import datetime, timezone
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from typing import Any
from urllib.error import HTTPError, URLError
from urllib.parse import parse_qs, urlencode, unquote, urlparse
from urllib.request import Request, urlopen
from xml.etree.ElementTree import Element, SubElement, tostring


ROOT = Path(__file__).resolve().parents[1]
WEB_ROOT = Path(__file__).resolve().parent / "web"
APP_DATA = Path(os.environ.get("LOCALAPPDATA", Path.home())) / "WonderCommandCompanion"
DATABASE_PATH = APP_DATA / "wonder_command.sqlite3"
CATEGORIES = {"medical", "social", "government", "identity", "game", "software"}
PRIVATE_CATEGORY = "identity"
STATUSES = {"Pending", "In Progress", "Completed"}
PRIORITIES = {"Urgent", "High", "Normal"}
HIDDEN_PARTS = {".git", ".svn", "__pycache__", "Saved", "Intermediate", "DerivedDataCache"}
SSN_PATTERN = re.compile(r"(?<!\d)(?:\d{3}[- ]?\d{2}[- ]?\d{4})(?!\d)")
MAX_BODY_SIZE = 256 * 1024
GEMINI_MODEL = "gemini-2.5-flash"
PERSONA_INSTRUCTIONS = {
    "mama": (
        "Speak as Mama: warm, steady, protective, encouraging, organized, and practical. "
        "Use an affectionate address sparingly. Do not claim to be a real person or a clinician."
    ),
    "papa": (
        "Speak as Papa: confident, energetic, strategic, ambitious, and practical. "
        "Focus on plans and useful next steps. Do not imitate any specific actor or copyrighted character."
    ),
    "both": (
        "Speak as Mama and Papa Co-Op: Mama speaks first with warmth, care, and practical organization, "
        "followed immediately by Papa with strategic energy, clear milestones, and actionable guidance."
    ),
}
ACTION_TYPES = {"CREATE_TASK", "SCAFFOLD_ENGINES", "GENERATE_SCRIPT"}
SCRIPT_DESTINATIONS = {
    "sync_wonder_pipeline.py": "WonderPipeline/sync_wonder_pipeline.py",
    "wonder_blender_ai_addon.py": "WonderPipeline/01_Blender_Source/Scripts/wonder_blender_ai_addon.py",
    "pack_orm_textures.py": "WonderPipeline/01_Blender_Source/Scripts/pack_orm_textures.py",
    "MamaTowerCities2System.cs": "WonderPipeline/04_Unity_Brawler_CS2/Mods_CitiesSkylines2/MamaTowerCities2System.cs",
}
ENGINE_DIRECTORIES = (
    "01_Blender_Source/Characters_Rigged",
    "01_Blender_Source/Tower_Modular_Meshes",
    "01_Blender_Source/Scripts",
    "02_Shared_Game_Core/State",
    "02_Shared_Game_Core/Exports_UE58_FBX",
    "02_Shared_Game_Core/Exports_Unity_FBX",
    "02_Shared_Game_Core/Textures_2K_4K_ORM",
    "03_UE58_WonderWorld",
    "04_Unity_Brawler_CS2/Mods_CitiesSkylines2",
)
LEDGER_TEMPLATE = {
    "schema_version": "3.0.0",
    "tower_name": "Mama Wonder Tower",
    "hardware_profile": "RTX_5060",
    "ledger_columns": {
        "column_1_gross_revenue": {},
        "column_2_operating_expenses": {},
        "column_3_net_reserve_and_upgrades": {
            "unlocked_tier": 1,
            "cities_skylines_lot_size": "8m x 8m",
            "emissive_signage_active": False,
        },
    },
}

SCRIPT_TEMPLATES = {
    "sync_wonder_pipeline.py": '''"""Validate a sidecar FBX manifest and sync shared ledger JSON without engine imports."""
import argparse
import json
import shutil
from pathlib import Path

EXPECTED_EXPORTS = {
    "ue58": ("-Y", "Z"),
    "unity": ("-Z", "Y"),
}

def validate_manifest(manifest):
    data = json.loads(manifest.read_text(encoding="utf-8"))
    target = data.get("target")
    if target not in EXPECTED_EXPORTS:
        raise ValueError("Manifest target must be ue58 or unity.")
    if (data.get("forward_axis"), data.get("up_axis")) != EXPECTED_EXPORTS[target]:
        raise ValueError("FBX axis metadata does not match the selected engine preset.")
    if not isinstance(data.get("scale_meters_per_unit"), (int, float)) or data["scale_meters_per_unit"] <= 0:
        raise ValueError("Set a positive scale_meters_per_unit value in the manifest.")
    source = (manifest.parent / data.get("file", "")).resolve()
    if manifest.parent.resolve() not in source.parents or not source.is_file():
        raise ValueError("Manifest FBX must be a file inside the manifest folder.")
    return source

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--manifest", type=Path, help="Sidecar JSON containing FBX export axis/scale metadata.")
    parser.add_argument("--ledger", type=Path, default=Path(__file__).parent / "02_Shared_Game_Core/State/WonderLedgerState.json")
    parser.add_argument("--ledger-copy", type=Path)
    args = parser.parse_args()
    if args.manifest:
        print("Validated FBX metadata:", validate_manifest(args.manifest))
    if args.ledger_copy:
        source = args.ledger.resolve()
        destination = args.ledger_copy.resolve()
        payload = json.loads(source.read_text(encoding="utf-8"))
        destination.parent.mkdir(parents=True, exist_ok=True)
        destination.write_text(json.dumps(payload, indent=2) + "\\\\n", encoding="utf-8")
        print("Copied validated ledger JSON to:", destination)

if __name__ == "__main__":
    main()
''',
    "wonder_blender_ai_addon.py": '''"""Blender 4.x helper panel for LODs, collision hulls, and dual FBX export."""
import bpy
from pathlib import Path

PIPELINE = Path(__file__).resolve().parents[2] / "02_Shared_Game_Core"

class WONDER_OT_make_lods(bpy.types.Operator):
    bl_idname = "wonder.make_lods"
    bl_label = "Create LOD0 / LOD1 / LOD2"
    bl_options = {"REGISTER", "UNDO"}

    def execute(self, context):
        source = context.active_object
        if source is None or source.type != "MESH":
            self.report({"ERROR"}, "Select one mesh object.")
            return {"CANCELLED"}
        for label, ratio in (("LOD0", 1.0), ("LOD1", 0.5), ("LOD2", 0.25)):
            duplicate = source.copy()
            duplicate.data = source.data.copy()
            duplicate.name = f"{source.name}_{label}"
            context.collection.objects.link(duplicate)
            if ratio < 1:
                modifier = duplicate.modifiers.new("Wonder Decimate", "DECIMATE")
                modifier.ratio = ratio
                bpy.context.view_layer.objects.active = duplicate
                bpy.ops.object.modifier_apply(modifier=modifier.name)
        return {"FINISHED"}

class WONDER_OT_make_collision(bpy.types.Operator):
    bl_idname = "wonder.make_collision"
    bl_label = "Create UCX Convex Hull"
    bl_options = {"REGISTER", "UNDO"}

    def execute(self, context):
        source = context.active_object
        if source is None or source.type != "MESH":
            self.report({"ERROR"}, "Select one mesh object.")
            return {"CANCELLED"}
        hull = source.copy()
        hull.data = source.data.copy()
        hull.name = f"UCX_{source.name}_00"
        context.collection.objects.link(hull)
        bpy.context.view_layer.objects.active = hull
        bpy.ops.object.select_all(action="DESELECT")
        hull.select_set(True)
        bpy.ops.object.mode_set(mode="EDIT")
        bpy.ops.mesh.select_all(action="SELECT")
        bpy.ops.mesh.convex_hull()
        bpy.ops.object.mode_set(mode="OBJECT")
        return {"FINISHED"}

class WONDER_OT_export_fbx(bpy.types.Operator):
    bl_idname = "wonder.export_fbx"
    bl_label = "Export Selected for UE5.8 and Unity"
    bl_options = {"REGISTER"}

    def execute(self, context):
        selected = context.selected_objects
        if not selected:
            self.report({"ERROR"}, "Select objects to export.")
            return {"CANCELLED"}
        for target, folder, forward, up in (
            ("UE58", "Exports_UE58_FBX", "-Y", "Z"),
            ("Unity", "Exports_Unity_FBX", "-Z", "Y"),
        ):
            destination = PIPELINE / folder
            destination.mkdir(parents=True, exist_ok=True)
            bpy.ops.export_scene.fbx(
                filepath=str(destination / f"{context.active_object.name}_{target}.fbx"),
                use_selection=True, axis_forward=forward, axis_up=up,
                apply_unit_scale=True, mesh_smooth_type="FACE",
            )
        return {"FINISHED"}

class WONDER_PT_panel(bpy.types.Panel):
    bl_label = "Wonder Command AI"
    bl_idname = "WONDER_PT_panel"
    bl_space_type = "VIEW_3D"
    bl_region_type = "UI"
    bl_category = "Wonder"

    def draw(self, context):
        layout = self.layout
        layout.operator("wonder.make_lods")
        layout.operator("wonder.make_collision")
        layout.operator("wonder.export_fbx")

CLASSES = (WONDER_OT_make_lods, WONDER_OT_make_collision, WONDER_OT_export_fbx, WONDER_PT_panel)

def register():
    for cls in CLASSES:
        bpy.utils.register_class(cls)

def unregister():
    for cls in reversed(CLASSES):
        bpy.utils.unregister_class(cls)

if __name__ == "__main__":
    register()
''',
    "pack_orm_textures.py": '''"""Pack grayscale AO, roughness, metallic images into ORM channels (R/G/B)."""
import argparse
from pathlib import Path

try:
    from PIL import Image
except ImportError as exc:
    raise SystemExit("Install Pillow for this standalone script: python -m pip install Pillow") from exc

def pack(ao_path, roughness_path, metallic_path, output_path):
    images = [Image.open(path).convert("L") for path in (ao_path, roughness_path, metallic_path)]
    if len({image.size for image in images}) != 1:
        raise ValueError("AO, roughness, and metallic images must have identical dimensions.")
    Image.merge("RGB", images).save(output_path)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--ao", type=Path, required=True)
    parser.add_argument("--roughness", type=Path, required=True)
    parser.add_argument("--metallic", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    args.output.parent.mkdir(parents=True, exist_ok=True)
    pack(args.ao, args.roughness, args.metallic, args.output)

if __name__ == "__main__":
    main()
''',
    "MamaTowerCities2System.cs": '''// Starter scaffold only: compile against the exact Cities: Skylines II SDK/version in use.
// The game currently uses Unity ECS / GameSystemBase, not generic Unity 2022.3 MonoBehaviour APIs.
using Game;
using Game.Simulation;
using Unity.Entities;

namespace WonderTower.CitiesSkylines2
{
    public partial class MamaTowerCities2System : GameSystemBase
    {
        private const int UpdatesPerDay = 16;
        private const int SimulationTicksPerDay = 262144;
        public const int LotWidthMeters = 8;
        public const int LotDepthMeters = 8;
        public const int ControlMaskResolution = 4096;
        public const int EmissiveResolution = 4096;

        protected override void OnCreate()
        {
            base.OnCreate();
            Enabled = false;
        }

        protected override void OnUpdate()
        {
            // Add version-matched ECS queries and game component access after SDK references are configured.
        }
    }
}
''',
}


def utc_now() -> str:
    return datetime.now(timezone.utc).isoformat(timespec="seconds")


def contains_ssn(value: str) -> bool:
    return bool(SSN_PATTERN.search(value))


def fallback_chat(mode: str, message: str) -> dict[str, Any]:
    preview = message.strip()[:160]
    reply = (
        "I have your request, but Gemini is not connected. Set GEMINI_API_KEY on the computer "
        "and try again. Your message was not sent to an AI service."
    )
    return {
        "mama_reply": reply if mode in {"mama", "both"} else "",
        "papa_reply": reply if mode in {"papa", "both"} else "",
        "proposed_action": None,
        "live_api": False,
        "message_preview": preview,
    }


def call_gemini_agent(mode: str, user_message: str) -> dict[str, Any]:
    api_key = os.environ.get("GEMINI_API_KEY", "").strip()
    if not api_key:
        return fallback_chat(mode, user_message)
    prompt = (
        f"{PERSONA_INSTRUCTIONS[mode]}\n"
        "Return valid JSON only, with keys mama_reply, papa_reply, proposed_action. "
        "Use an empty string for the persona that is not active. proposed_action must be null "
        "or an object with action_type and payload. Allowed action_type values are "
        "CREATE_TASK, SCAFFOLD_ENGINES, and GENERATE_SCRIPT. For CREATE_TASK, payload contains "
        "category and title. For GENERATE_SCRIPT, payload contains only script_name selected "
        "from sync_wonder_pipeline.py, wonder_blender_ai_addon.py, pack_orm_textures.py, "
        "MamaTowerCities2System.cs. Never include personal identifiers or sensitive details "
        "in an action. Do not execute or claim to have executed actions.\n"
        f"User message:\n{user_message}"
    )
    payload = {
        "contents": [{"parts": [{"text": prompt}]}],
        "generationConfig": {"responseMimeType": "application/json", "temperature": 0.7},
    }
    url = (
        "https://generativelanguage.googleapis.com/v1beta/models/"
        f"{GEMINI_MODEL}:generateContent?{urlencode({'key': api_key})}"
    )
    request = Request(
        url,
        data=json.dumps(payload).encode("utf-8"),
        headers={"Content-Type": "application/json"},
        method="POST",
    )
    try:
        with urlopen(request, timeout=20) as response:
            data = json.loads(response.read().decode("utf-8"))
        raw_text = data["candidates"][0]["content"]["parts"][0]["text"]
        parsed = json.loads(raw_text)
        mama_reply = str(parsed.get("mama_reply", ""))[:4000]
        papa_reply = str(parsed.get("papa_reply", ""))[:4000]
        if mode == "mama":
            papa_reply = ""
        elif mode == "papa":
            mama_reply = ""
        return {
            "mama_reply": mama_reply,
            "papa_reply": papa_reply,
            "proposed_action": parsed.get("proposed_action"),
            "live_api": True,
        }
    except (HTTPError, URLError, TimeoutError, OSError, KeyError, IndexError, TypeError, json.JSONDecodeError):
        return {
            "mama_reply": "The connection to the AI service did not complete. Please check the network and API key.",
            "papa_reply": "Nothing was executed or saved to an external action. You can retry when the connection is ready.",
            "proposed_action": None,
            "live_api": False,
        }


class Store:
    def __init__(self, path: Path, project_root: Path):
        self.path = path
        self.project_root = project_root.resolve()
        self.lock = threading.RLock()
        self.path.parent.mkdir(parents=True, exist_ok=True)
        with self.connect() as db:
            db.executescript(
                """
                CREATE TABLE IF NOT EXISTS tasks (
                    id TEXT PRIMARY KEY,
                    category TEXT NOT NULL,
                    title TEXT NOT NULL,
                    priority TEXT NOT NULL,
                    status TEXT NOT NULL,
                    action TEXT NOT NULL,
                    updated_at TEXT NOT NULL
                );
                CREATE TABLE IF NOT EXISTS activity (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    event TEXT NOT NULL,
                    category TEXT NOT NULL,
                    details TEXT NOT NULL,
                    created_at TEXT NOT NULL
                );
                CREATE TABLE IF NOT EXISTS agent_queue (
                    id TEXT PRIMARY KEY,
                    category TEXT NOT NULL,
                    prompt TEXT NOT NULL,
                    status TEXT NOT NULL,
                    created_at TEXT NOT NULL
                );
                CREATE TABLE IF NOT EXISTS file_operations (
                    id TEXT PRIMARY KEY,
                    action TEXT NOT NULL,
                    source TEXT NOT NULL,
                    destination TEXT NOT NULL,
                    status TEXT NOT NULL,
                    created_at TEXT NOT NULL
                );
                CREATE TABLE IF NOT EXISTS pending_actions (
                    id TEXT PRIMARY KEY,
                    action_type TEXT NOT NULL,
                    payload TEXT NOT NULL,
                    status TEXT NOT NULL,
                    created_at TEXT NOT NULL,
                    result TEXT NOT NULL DEFAULT ''
                );
                """
            )

    @contextmanager
    def connect(self):
        db = sqlite3.connect(self.path, timeout=10)
        db.row_factory = sqlite3.Row
        try:
            yield db
            db.commit()
        except Exception:
            db.rollback()
            raise
        finally:
            db.close()

    def log(self, event: str, category: str, details: str) -> None:
        entry = {
            "event": event,
            "category": category,
            "details": details[:500],
            "created_at": utc_now(),
        }
        with self.lock, self.connect() as db:
            db.execute(
                "INSERT INTO activity (event, category, details, created_at) VALUES (?, ?, ?, ?)",
                (entry["event"], entry["category"], entry["details"], entry["created_at"]),
            )
            with self.path.with_suffix(".jsonl").open("a", encoding="utf-8") as log_file:
                log_file.write(json.dumps(entry, ensure_ascii=False) + "\n")

    def list_tasks(self, include_private: bool = True) -> list[dict[str, Any]]:
        with self.lock, self.connect() as db:
            rows = db.execute("SELECT * FROM tasks ORDER BY updated_at DESC").fetchall()
        tasks = [dict(row) for row in rows]
        if not include_private:
            tasks = [task for task in tasks if task["category"] != PRIVATE_CATEGORY]
        return tasks

    def save_tasks(self, tasks: list[dict[str, Any]], include_private: bool = True) -> None:
        if not isinstance(tasks, list) or len(tasks) > 500:
            raise ValueError("Tasks must be a list containing at most 500 entries.")
        prepared = []
        for task in tasks:
            if not isinstance(task, dict):
                raise ValueError("Each task must be an object.")
            category = str(task.get("category", task.get("cat", "")))
            title = str(task.get("title", "")).strip()
            task_id = str(task.get("id") or uuid.uuid4())
            priority = str(task.get("priority", "Normal"))
            status = str(task.get("status", "Pending"))
            action = str(task.get("action", "")).strip()
            if category not in CATEGORIES or not title or len(title) > 500:
                raise ValueError("Task category or title is invalid.")
            if priority not in PRIORITIES or status not in STATUSES:
                raise ValueError("Task priority or status is invalid.")
            if not include_private and category == PRIVATE_CATEGORY:
                raise PermissionError("Private identity tasks cannot be changed from a remote device.")
            if contains_ssn(title) or contains_ssn(action):
                raise ValueError("Do not enter a Social Security number in task data.")
            prepared.append((task_id, category, title, priority, status, action[:1000], utc_now()))

        with self.lock, self.connect() as db:
            if include_private:
                db.execute("DELETE FROM tasks")
            else:
                db.execute("DELETE FROM tasks WHERE category != ?", (PRIVATE_CATEGORY,))
            db.executemany(
                """INSERT INTO tasks
                   (id, category, title, priority, status, action, updated_at)
                   VALUES (?, ?, ?, ?, ?, ?, ?)
                   ON CONFLICT(id) DO UPDATE SET
                     category=excluded.category, title=excluded.title,
                     priority=excluded.priority, status=excluded.status,
                     action=excluded.action, updated_at=excluded.updated_at""",
                prepared,
            )
        self.log("tasks_saved", "software", f"Saved {len(prepared)} task(s).")

    def create_agent_request(self, category: str, prompt: str) -> str:
        if category not in CATEGORIES:
            raise ValueError("Choose a supported task category.")
        if category == PRIVATE_CATEGORY:
            raise PermissionError("Identity tasks are excluded from AI handoff.")
        prompt = prompt.strip()
        if not prompt or len(prompt) > 8000:
            raise ValueError("Prompt must contain 1 to 8000 characters.")
        if contains_ssn(prompt):
            raise ValueError("Remove Social Security numbers before sending an agent request.")
        request_id = str(uuid.uuid4())
        with self.lock, self.connect() as db:
            db.execute(
                "INSERT INTO agent_queue (id, category, prompt, status, created_at) VALUES (?, ?, ?, ?, ?)",
                (request_id, category, prompt, "queued", utc_now()),
            )
        self.log("agent_queued", category, f"Agent request queued: {request_id}")
        return request_id

    def create_chat_action(self, action: Any) -> str | None:
        if action is None:
            return None
        if not isinstance(action, dict):
            raise ValueError("The proposed action must be an object or null.")
        action_type = action.get("action_type")
        payload = action.get("payload", {})
        if action_type not in ACTION_TYPES or not isinstance(payload, dict):
            raise ValueError("The proposed action type or payload is invalid.")
        safe_payload: dict[str, Any]
        if action_type == "CREATE_TASK":
            category = str(payload.get("category", "software"))
            title = str(payload.get("title", "")).strip()
            if category not in CATEGORIES - {PRIVATE_CATEGORY} or not title or len(title) > 250:
                raise ValueError("The proposed task category or title is invalid.")
            if contains_ssn(title):
                raise ValueError("A task containing sensitive number patterns cannot be queued.")
            safe_payload = {"category": category, "title": title}
        elif action_type == "SCAFFOLD_ENGINES":
            safe_payload = {}
        else:
            script_name = str(payload.get("script_name", ""))
            if script_name not in SCRIPT_DESTINATIONS:
                raise ValueError("The script is not in the approved script catalog.")
            safe_payload = {"script_name": script_name}
        action_id = str(uuid.uuid4())
        with self.lock, self.connect() as db:
            db.execute(
                """INSERT INTO pending_actions
                   (id, action_type, payload, status, created_at)
                   VALUES (?, ?, ?, 'pending_approval', ?)""",
                (action_id, action_type, json.dumps(safe_payload), utc_now()),
            )
        self.log("action_pending_approval", "software", f"Action {action_id} awaits user approval.")
        return action_id

    def list_pending_actions(self, limit: int = 100) -> list[dict[str, Any]]:
        with self.lock, self.connect() as db:
            rows = db.execute(
                """SELECT id, action_type, payload, status, created_at
                   FROM pending_actions WHERE status = 'pending_approval'
                   ORDER BY created_at DESC LIMIT ?""",
                (max(1, min(limit, 500)),),
            ).fetchall()
        actions = []
        for row in rows:
            item = dict(row)
            item["payload"] = json.loads(item["payload"])
            actions.append(item)
        return actions

    def _write_new_file(self, relative_path: str, content: str) -> None:
        destination = self.safe_project_path(relative_path, allow_missing=True)
        destination.parent.mkdir(parents=True, exist_ok=True)
        with destination.open("x", encoding="utf-8", newline="\n") as output:
            output.write(content)

    def scaffold_engines(self) -> list[str]:
        for relative in ENGINE_DIRECTORIES:
            destination = self.safe_project_path("WonderPipeline/" + relative, allow_missing=True)
            destination.mkdir(parents=True, exist_ok=True)
        outputs = {
            "WonderPipeline/02_Shared_Game_Core/State/WonderLedgerState.json":
                json.dumps(LEDGER_TEMPLATE, indent=2) + "\n",
            "WonderPipeline/03_UE58_WonderWorld/MCP_Bridge_Config.json":
                json.dumps({"server": "WonderTowerTycoon", "engine": "Unreal Engine 5.8", "port": 8000}, indent=2) + "\n",
            **{
                SCRIPT_DESTINATIONS[name]: source
                for name, source in SCRIPT_TEMPLATES.items()
            },
        }
        created = []
        for path, content in outputs.items():
            try:
                self._write_new_file(path, content)
                created.append(path)
            except FileExistsError:
                continue
        return created

    def approve_action(self, action_id: str) -> list[str]:
        with self.lock, self.connect() as db:
            row = db.execute(
                "SELECT * FROM pending_actions WHERE id = ? AND status = 'pending_approval'",
                (action_id,),
            ).fetchone()
            if row is None:
                raise LookupError("No pending action was found.")
            payload = json.loads(row["payload"])
            action_type = row["action_type"]
            if action_type == "CREATE_TASK":
                category = payload["category"]
                title = payload["title"]
                task_id = str(uuid.uuid4())
                db.execute(
                    """INSERT INTO tasks
                       (id, category, title, priority, status, action, updated_at)
                       VALUES (?, ?, ?, 'Normal', 'Pending', ?, ?)""",
                    (task_id, category, title, title, utc_now()),
                )
                created = [f"Task: {title}"]
            elif action_type == "SCAFFOLD_ENGINES":
                created = self.scaffold_engines()
            elif action_type == "GENERATE_SCRIPT":
                name = payload["script_name"]
                destination = SCRIPT_DESTINATIONS[name]
                target = self.safe_project_path(destination, allow_missing=True)
                target.parent.mkdir(parents=True, exist_ok=True)
                self._write_new_file(destination, SCRIPT_TEMPLATES[name])
                created = [destination]
            else:
                raise ValueError("Unsupported action.")
            result = json.dumps(created)
            db.execute(
                "UPDATE pending_actions SET status = 'approved', result = ? WHERE id = ?",
                (result, action_id),
            )
        self.log("action_approved", "software", f"Approved action {action_id}: {action_type}.")
        return created

    def chat_log(self, mode: str, request_id: str, status: str) -> None:
        # Conversation content stays in the browser; persistent logs contain no prompt or response.
        self.log("agent_chat", "software", f"Chat {request_id} ({mode}) completed: {status}.")

    def convert_schema(self, data: Any, output_format: str, mask_sensitive: bool) -> str:
        if output_format not in {"json", "xml", "md", "csv"}:
            raise ValueError("Choose json, xml, md, or csv.")
        if len(json.dumps(data, ensure_ascii=False)) > MAX_BODY_SIZE:
            raise ValueError("Conversion data is too large.")
        text = json.dumps(data, ensure_ascii=False)
        if mask_sensitive:
            text = SSN_PATTERN.sub("[REDACTED-SSN]", text)
            data = json.loads(text)
        if output_format == "json":
            return json.dumps(data, ensure_ascii=False, indent=2)
        if output_format == "md":
            if isinstance(data, dict):
                return "\n".join(f"- **{key}:** {value}" for key, value in data.items())
            if isinstance(data, list):
                return "\n".join(f"- {json.dumps(item, ensure_ascii=False)}" for item in data)
            return str(data)
        if output_format == "csv":
            records = data if isinstance(data, list) else [data]
            if not all(isinstance(item, dict) for item in records):
                records = [{"value": item} for item in records]
            columns = sorted({key for item in records for key in item})
            output = io.StringIO(newline="")
            writer = csv.DictWriter(output, fieldnames=columns or ["value"])
            writer.writeheader()
            writer.writerows(records)
            return output.getvalue()
        root = Element("wonder_command_export")
        records = data if isinstance(data, list) else [data]
        for record in records:
            node = SubElement(root, "record")
            fields = record.items() if isinstance(record, dict) else [("value", record)]
            for key, value in fields:
                field = SubElement(node, re.sub(r"[^A-Za-z0-9_.-]", "_", str(key)) or "field")
                field.text = json.dumps(value, ensure_ascii=False) if isinstance(value, (dict, list)) else str(value)
        return tostring(root, encoding="unicode")

    def list_activity(self, limit: int = 100) -> list[dict[str, Any]]:
        with self.lock, self.connect() as db:
            rows = db.execute(
                "SELECT event, category, details, created_at FROM activity ORDER BY id DESC LIMIT ?",
                (max(1, min(limit, 500)),),
            ).fetchall()
        return [dict(row) for row in rows]

    def list_agent_queue(self, limit: int = 100) -> list[dict[str, Any]]:
        with self.lock, self.connect() as db:
            rows = db.execute(
                """SELECT id, category, prompt, status, created_at
                   FROM agent_queue ORDER BY created_at DESC LIMIT ?""",
                (max(1, min(limit, 500)),),
            ).fetchall()
        return [dict(row) for row in rows]

    def safe_project_path(self, relative_path: str, allow_missing: bool = False) -> Path:
        if not isinstance(relative_path, str) or not relative_path or Path(relative_path).is_absolute():
            raise ValueError("Use a non-empty path relative to the project folder.")
        candidate = (self.project_root / relative_path).resolve(strict=False)
        try:
            candidate.relative_to(self.project_root)
        except ValueError as exc:
            raise ValueError("Path must stay inside the configured project folder.") from exc
        if any(part in HIDDEN_PARTS for part in candidate.relative_to(self.project_root).parts):
            raise ValueError("This protected project folder is not available through the companion.")
        if not allow_missing and not candidate.exists():
            raise ValueError("Project path does not exist.")
        return candidate

    def list_files(self, relative_path: str = ".") -> list[dict[str, Any]]:
        folder = self.project_root if relative_path == "." else self.safe_project_path(relative_path)
        if not folder.is_dir():
            raise ValueError("Choose a project folder.")
        results = []
        for item in sorted(folder.iterdir(), key=lambda entry: (not entry.is_dir(), entry.name.casefold())):
            if item.name in HIDDEN_PARTS or item.is_symlink():
                continue
            try:
                results.append(
                    {
                        "name": item.name,
                        "path": item.relative_to(self.project_root).as_posix(),
                        "type": "folder" if item.is_dir() else "file",
                        "size": item.stat().st_size if item.is_file() else None,
                    }
                )
            except OSError:
                continue
        return results

    def stage_file_operation(self, action: str, source: str, destination: str) -> str:
        if action not in {"copy", "move"}:
            raise ValueError("Only copy and move operations are supported.")
        source_path = self.safe_project_path(source)
        destination_path = self.safe_project_path(destination, allow_missing=True)
        if not source_path.is_file():
            raise ValueError("Only individual files can be copied or moved.")
        if source_path == destination_path:
            raise ValueError("Source and destination must be different.")
        if destination_path.exists():
            raise FileExistsError("Destination already exists; overwrite is not allowed.")
        operation_id = str(uuid.uuid4())
        with self.lock, self.connect() as db:
            db.execute(
                """INSERT INTO file_operations
                   (id, action, source, destination, status, created_at)
                   VALUES (?, ?, ?, ?, ?, ?)""",
                (operation_id, action, source, destination, "pending_approval", utc_now()),
            )
        self.log("file_operation_staged", "software", f"{action} request {operation_id} awaits approval.")
        return operation_id

    def approve_file_operation(self, operation_id: str) -> None:
        with self.lock, self.connect() as db:
            row = db.execute(
                "SELECT * FROM file_operations WHERE id = ? AND status = 'pending_approval'",
                (operation_id,),
            ).fetchone()
            if row is None:
                raise LookupError("No pending file operation was found.")
            source = self.safe_project_path(row["source"])
            destination = self.safe_project_path(row["destination"], allow_missing=True)
            if not source.is_file() or destination.exists():
                raise FileExistsError("The source changed or the destination already exists.")
            destination.parent.mkdir(parents=True, exist_ok=True)
            if row["action"] == "copy":
                shutil.copy2(source, destination)
            else:
                shutil.move(str(source), str(destination))
            db.execute(
                "UPDATE file_operations SET status = 'approved' WHERE id = ?",
                (operation_id,),
            )
        self.log("file_operation_approved", "software", f"{row['action']} request {operation_id} completed.")


class CompanionHandler(BaseHTTPRequestHandler):
    store: Store
    token_digest: bytes

    server_version = "WonderCommandCompanion/1.0"

    def log_message(self, fmt: str, *args: Any) -> None:
        sys.stderr.write(f"{self.address_string()} - {fmt % args}\n")

    @property
    def remote_client(self) -> bool:
        return self.client_address[0] not in {"127.0.0.1", "::1"}

    def send_json(self, status: int, payload: Any) -> None:
        body = json.dumps(payload, ensure_ascii=False).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Cache-Control", "no-store")
        self.end_headers()
        self.wfile.write(body)

    def read_json(self) -> Any:
        try:
            length = int(self.headers.get("Content-Length", "0"))
        except ValueError as exc:
            raise ValueError("Invalid content length.") from exc
        if length < 1 or length > MAX_BODY_SIZE:
            raise ValueError("Request body must be between 1 byte and 256 KB.")
        try:
            return json.loads(self.rfile.read(length))
        except (json.JSONDecodeError, UnicodeDecodeError) as exc:
            raise ValueError("Request body must be valid JSON.") from exc

    def authorized(self) -> bool:
        supplied = self.headers.get("Authorization", "")
        if not supplied.startswith("Bearer "):
            return False
        candidate = hashlib.sha256(supplied[7:].encode("utf-8")).digest()
        return hmac.compare_digest(candidate, self.token_digest)

    def do_GET(self) -> None:
        parsed = urlparse(self.path)
        if parsed.path == "/health":
            self.send_json(200, {"ok": True, "service": "Wonder Command Companion"})
            return
        if parsed.path == "/":
            self.serve_static("pair.html" if self.remote_client else "index.html")
            return
        if parsed.path == "/dashboard":
            if not self.authorized():
                self.send_json(401, {"error": "Enter the companion pairing token."})
                return
            self.serve_static("index.html")
            return
        if parsed.path.startswith("/api/") and not self.authorized():
            self.send_json(401, {"error": "Enter the companion pairing token."})
            return
        if parsed.path == "/api/status":
            self.send_json(200, {"ok": True, "version": "1.0", "remote": self.remote_client})
        elif parsed.path == "/api/tasks":
            self.send_json(200, {"tasks": self.store.list_tasks(include_private=not self.remote_client)})
        elif parsed.path == "/api/activity":
            self.send_json(200, {"activity": self.store.list_activity()})
        elif parsed.path == "/api/agent-queue":
            self.send_json(200, {"requests": self.store.list_agent_queue()})
        elif parsed.path == "/api/actions":
            self.send_json(200, {"actions": self.store.list_pending_actions()})
        elif parsed.path == "/api/files":
            query = parse_qs(parsed.query)
            try:
                self.send_json(200, {"files": self.store.list_files(query.get("path", ["."])[0])})
            except (ValueError, OSError) as exc:
                self.send_json(400, {"error": str(exc)})
        elif parsed.path == "/api/logs/export":
            payload = "\n".join(json.dumps(item, ensure_ascii=False) for item in self.store.list_activity())
            body = (payload + "\n").encode("utf-8") if payload else b""
            self.send_response(200)
            self.send_header("Content-Type", "application/x-ndjson; charset=utf-8")
            self.send_header("Content-Disposition", 'attachment; filename="wonder-command-activity.jsonl"')
            self.send_header("Content-Length", str(len(body)))
            self.send_header("Cache-Control", "no-store")
            self.end_headers()
            self.wfile.write(body)
        else:
            if self.remote_client and not self.authorized():
                self.send_json(401, {"error": "Pair before accessing the dashboard."})
                return
            self.serve_static()

    def do_POST(self) -> None:
        if not self.authorized():
            self.send_json(401, {"error": "Enter the companion pairing token."})
            return
        parsed = urlparse(self.path)
        try:
            data = self.read_json()
            if not isinstance(data, dict):
                raise ValueError("Request must be a JSON object.")
            if parsed.path == "/api/agent":
                category = str(data.get("category", "software"))
                if self.remote_client and category == PRIVATE_CATEGORY:
                    raise PermissionError("Private identity tasks cannot be sent from a remote device.")
                request_id = self.store.create_agent_request(category, str(data.get("prompt", "")))
                self.send_json(
                    202,
                    {
                        "id": request_id,
                        "status": "queued",
                        "message": "Saved to the local queue. No AI provider is connected; nothing was executed.",
                    },
                )
            elif parsed.path == "/api/agent/chat":
                mode = str(data.get("mode", "both"))
                message = str(data.get("message", "")).strip()
                if mode not in PERSONA_INSTRUCTIONS:
                    raise ValueError("Choose mama, papa, or both.")
                if data.get("external_consent") is not True:
                    raise PermissionError("Confirm that this message may be sent to Google Gemini.")
                if not message or len(message) > 8000:
                    raise ValueError("Message must contain 1 to 8000 characters.")
                if contains_ssn(message):
                    raise ValueError("Remove Social Security numbers before using AI chat.")
                request_id = str(uuid.uuid4())
                result = call_gemini_agent(mode, message)
                action = result.pop("proposed_action", None)
                action_id = self.store.create_chat_action(action)
                result["pending_action_id"] = action_id
                self.store.chat_log(mode, request_id, "sent_to_gemini" if result["live_api"] else "offline")
                self.send_json(200, result)
            elif parsed.path == "/api/actions/approve":
                action_id = str(data.get("id", ""))
                created = self.store.approve_action(action_id)
                self.send_json(200, {"id": action_id, "status": "approved", "created": created})
            elif parsed.path == "/api/engine/scaffold":
                action_id = self.store.create_chat_action(
                    {"action_type": "SCAFFOLD_ENGINES", "payload": {}}
                )
                self.send_json(202, {"id": action_id, "status": "pending_approval"})
            elif parsed.path == "/api/engine/generate-script":
                script_name = str(data.get("script_name", ""))
                action_id = self.store.create_chat_action(
                    {"action_type": "GENERATE_SCRIPT", "payload": {"script_name": script_name}}
                )
                self.send_json(202, {"id": action_id, "status": "pending_approval"})
            elif parsed.path == "/api/convert/schema":
                converted = self.store.convert_schema(
                    data.get("data"),
                    str(data.get("format", "")),
                    data.get("mask_sensitive") is True,
                )
                self.send_json(
                    200,
                    {"format": str(data.get("format")), "content": converted},
                )
            elif parsed.path == "/api/file-operations":
                request_id = self.store.stage_file_operation(
                    str(data.get("action", "")),
                    str(data.get("source", "")),
                    str(data.get("destination", "")),
                )
                self.send_json(202, {"id": request_id, "status": "pending_approval"})
            elif parsed.path.startswith("/api/file-operations/") and parsed.path.endswith("/approve"):
                operation_id = parsed.path.split("/")[-2]
                self.store.approve_file_operation(operation_id)
                self.send_json(200, {"id": operation_id, "status": "approved"})
            else:
                self.send_json(404, {"error": "Unknown API route."})
        except PermissionError as exc:
            self.send_json(403, {"error": str(exc)})
        except ValueError as exc:
            self.send_json(400, {"error": str(exc)})
        except FileExistsError as exc:
            self.send_json(409, {"error": str(exc)})
        except LookupError as exc:
            self.send_json(404, {"error": str(exc)})
        except OSError as exc:
            self.send_json(500, {"error": f"File operation failed: {exc}"})

    def do_PUT(self) -> None:
        if not self.authorized():
            self.send_json(401, {"error": "Enter the companion pairing token."})
            return
        if urlparse(self.path).path != "/api/tasks":
            self.send_json(404, {"error": "Unknown API route."})
            return
        try:
            data = self.read_json()
            if not isinstance(data, dict) or not isinstance(data.get("tasks"), list):
                raise ValueError("Request must contain a tasks list.")
            self.store.save_tasks(data["tasks"], include_private=not self.remote_client)
            self.send_json(200, {"tasks": self.store.list_tasks(include_private=not self.remote_client)})
        except PermissionError as exc:
            self.send_json(403, {"error": str(exc)})
        except ValueError as exc:
            self.send_json(400, {"error": str(exc)})

    def serve_static(self, requested_path: str | None = None) -> None:
        relative = requested_path or unquote(urlparse(self.path).path).lstrip("/")
        candidate = (WEB_ROOT / relative).resolve()
        try:
            candidate.relative_to(WEB_ROOT.resolve())
        except ValueError:
            self.send_json(404, {"error": "Not found."})
            return
        if not candidate.is_file():
            self.send_json(404, {"error": "Not found."})
            return
        content_type = "text/html; charset=utf-8" if candidate.suffix.lower() == ".html" else "application/octet-stream"
        body = candidate.read_bytes()
        self.send_response(200)
        self.send_header("Content-Type", content_type)
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Cache-Control", "no-store")
        self.end_headers()
        self.wfile.write(body)


def main() -> None:
    parser = argparse.ArgumentParser(description="Run the local Wonder Command desktop/phone companion.")
    parser.add_argument("--host", default="127.0.0.1", help="Bind address; default is this computer only.")
    parser.add_argument("--port", type=int, default=8765, help="Local web port (default: 8765).")
    parser.add_argument("--project-root", type=Path, default=ROOT, help="Project folder allowed for file browsing.")
    parser.add_argument("--database", type=Path, default=DATABASE_PATH, help="SQLite database path.")
    args = parser.parse_args()

    token = secrets.token_urlsafe(24)
    handler = type(
        "ConfiguredCompanionHandler",
        (CompanionHandler,),
        {
            "store": Store(args.database.expanduser(), args.project_root.expanduser()),
            "token_digest": hashlib.sha256(token.encode("utf-8")).digest(),
        },
    )
    server = ThreadingHTTPServer((args.host, args.port), handler)
    server.daemon_threads = True
    print(f"Wonder Command Companion: http://{args.host}:{args.port}/")
    print(f"Pairing token (shown once; enter it in the dashboard): {token}")
    if args.host not in {"127.0.0.1", "::1", "localhost"}:
        print("WARNING: LAN mode uses plain HTTP. Use only a trusted network; prefer a VPN/HTTPS tunnel.")
        print("Identity tasks are hidden from remote devices and blocked from agent handoff.")
    print(f"File access is limited to: {args.project_root.resolve()}")
    print("Press Ctrl+C to stop.")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nStopping companion.")
    finally:
        server.server_close()


if __name__ == "__main__":
    main()
