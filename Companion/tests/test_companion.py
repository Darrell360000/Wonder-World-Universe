import hashlib
import http.client
import json
import threading
import tempfile
import unittest
from pathlib import Path
from http.server import ThreadingHTTPServer

from Companion.wonder_command_companion import CompanionHandler, SCRIPT_TEMPLATES, Store


class StoreTests(unittest.TestCase):
    def setUp(self):
        self.temp_dir = tempfile.TemporaryDirectory()
        self.root = Path(self.temp_dir.name) / "project"
        self.root.mkdir()
        self.store = Store(Path(self.temp_dir.name) / "data.sqlite3", self.root)

    def tearDown(self):
        self.temp_dir.cleanup()

    def test_remote_task_list_excludes_identity_category(self):
        self.store.save_tasks(
            [
                {"id": 1, "cat": "identity", "title": "Private record reminder"},
                {"id": 2, "cat": "game", "title": "Build a test level"},
            ]
        )

        self.assertEqual(
            [task["category"] for task in self.store.list_tasks(include_private=False)],
            ["game"],
        )

    def test_identity_category_and_ssn_are_rejected_from_agent_queue(self):
        with self.assertRaises(PermissionError):
            self.store.create_agent_request("identity", "Review identity documents")
        with self.assertRaises(ValueError):
            self.store.create_agent_request("government", "SSN 123-45-6789")

    def test_remote_cannot_save_identity_task(self):
        with self.assertRaises(PermissionError):
            self.store.save_tasks(
                [{"id": 1, "cat": "identity", "title": "Private record"}],
                include_private=False,
            )

    def test_paths_cannot_escape_project_root(self):
        with self.assertRaises(ValueError):
            self.store.safe_project_path("../outside.txt")

    def test_file_operation_requires_approval_and_copies_without_overwrite(self):
        source = self.root / "source.txt"
        source.write_text("asset", encoding="utf-8")
        operation_id = self.store.stage_file_operation(
            "copy", "source.txt", "Exports/copied.txt"
        )
        self.assertFalse((self.root / "Exports" / "copied.txt").exists())

        self.store.approve_file_operation(operation_id)

        self.assertEqual((self.root / "Exports" / "copied.txt").read_text(encoding="utf-8"), "asset")
        self.assertTrue(source.exists())

    def test_hidden_project_folders_are_not_listed_or_accessible(self):
        (self.root / "Saved").mkdir()
        with self.assertRaises(ValueError):
            self.store.safe_project_path("Saved")
        self.assertEqual(self.store.list_files("."), [])

    def test_chat_actions_require_approval_and_use_fixed_script_destinations(self):
        action_id = self.store.create_chat_action(
            {
                "action_type": "GENERATE_SCRIPT",
                "payload": {"script_name": "pack_orm_textures.py"},
            }
        )
        destination = self.root / "WonderPipeline" / "01_Blender_Source" / "Scripts" / "pack_orm_textures.py"
        self.assertFalse(destination.exists())
        self.assertEqual(len(self.store.list_pending_actions()), 1)

        created = self.store.approve_action(action_id)

        self.assertEqual(created, ["WonderPipeline/01_Blender_Source/Scripts/pack_orm_textures.py"])
        self.assertTrue(destination.is_file())
        self.assertEqual(self.store.list_pending_actions(), [])

    def test_chat_action_rejects_private_or_arbitrary_actions(self):
        with self.assertRaises(ValueError):
            self.store.create_chat_action(
                {
                    "action_type": "GENERATE_SCRIPT",
                    "payload": {"script_name": "../outside.py"},
                }
            )
        with self.assertRaises(ValueError):
            self.store.create_chat_action(
                {
                    "action_type": "CREATE_TASK",
                    "payload": {"category": "identity", "title": "Private record"},
                }
            )

    def test_schema_conversion_masks_ssn_in_supported_formats(self):
        data = {"notes": "Example 123-45-6789"}
        for output_format in ("json", "xml", "md", "csv"):
            with self.subTest(output_format=output_format):
                result = self.store.convert_schema(data, output_format, mask_sensitive=True)
                self.assertNotIn("123-45-6789", result)
                self.assertIn("[REDACTED-SSN]", result)

    def test_chat_activity_log_contains_metadata_not_message_content(self):
        self.store.chat_log("mama", "request-1", "offline")
        activity = self.store.list_activity()
        self.assertEqual(activity[0]["event"], "agent_chat")
        self.assertNotIn("private message", activity[0]["details"])
        log_content = self.store.path.with_suffix(".jsonl").read_text(encoding="utf-8")
        self.assertIn("agent_chat", log_content)
        self.assertNotIn("private message", log_content)

    def test_python_starter_scripts_are_syntactically_valid(self):
        for name in (
            "sync_wonder_pipeline.py",
            "wonder_blender_ai_addon.py",
            "pack_orm_textures.py",
        ):
            with self.subTest(script=name):
                compile(SCRIPT_TEMPLATES[name], name, "exec")

    def test_api_requires_token_and_reports_local_connection(self):
        token = "local-test-token"
        handler = type(
            "TestHandler",
            (CompanionHandler,),
            {
                "store": self.store,
                "token_digest": hashlib.sha256(token.encode("utf-8")).digest(),
            },
        )
        server = ThreadingHTTPServer(("127.0.0.1", 0), handler)
        worker = threading.Thread(target=server.serve_forever, daemon=True)
        worker.start()
        try:
            connection = http.client.HTTPConnection("127.0.0.1", server.server_port)
            connection.request("GET", "/api/status")
            response = connection.getresponse()
            self.assertEqual(response.status, 401)
            response.read()

            connection.request(
                "GET",
                "/api/status",
                headers={"Authorization": f"Bearer {token}"},
            )
            response = connection.getresponse()
            self.assertEqual(response.status, 200)
            self.assertEqual(response.read(), b'{"ok": true, "version": "1.0", "remote": false}')

            connection.request(
                "POST",
                "/api/agent/chat",
                body=json.dumps({"mode": "mama", "message": "Hello", "external_consent": False}),
                headers={
                    "Authorization": f"Bearer {token}",
                    "Content-Type": "application/json",
                },
            )
            response = connection.getresponse()
            self.assertEqual(response.status, 403)
            response.read()

            connection.request(
                "POST",
                "/api/agent/chat",
                body=json.dumps({"mode": "mama", "message": "123-45-6789", "external_consent": True}),
                headers={
                    "Authorization": f"Bearer {token}",
                    "Content-Type": "application/json",
                },
            )
            response = connection.getresponse()
            self.assertEqual(response.status, 400)
            response.read()

            connection.request("GET", "/")
            response = connection.getresponse()
            self.assertEqual(response.status, 200)
            dashboard = response.read()
            self.assertIn(b'id="companion-connect-btn"', dashboard)
            self.assertIn(b'id="tab-government"', dashboard)
            connection.close()
        finally:
            server.shutdown()
            server.server_close()
            worker.join(timeout=2)

    def test_remote_must_pair_before_dashboard_is_served(self):
        token = "remote-test-token"
        handler = type(
            "RemoteTestHandler",
            (CompanionHandler,),
            {
                "store": self.store,
                "token_digest": hashlib.sha256(token.encode("utf-8")).digest(),
                "remote_client": property(lambda _self: True),
            },
        )
        server = ThreadingHTTPServer(("127.0.0.1", 0), handler)
        worker = threading.Thread(target=server.serve_forever, daemon=True)
        worker.start()
        try:
            connection = http.client.HTTPConnection("127.0.0.1", server.server_port)
            connection.request("GET", "/")
            response = connection.getresponse()
            pairing_page = response.read()
            self.assertEqual(response.status, 200)
            self.assertIn(b'id="pair-form"', pairing_page)
            self.assertNotIn(b'id="ssn-display"', pairing_page)

            connection.request("GET", "/dashboard")
            response = connection.getresponse()
            self.assertEqual(response.status, 401)
            response.read()

            connection.request(
                "GET",
                "/dashboard",
                headers={"Authorization": f"Bearer {token}"},
            )
            response = connection.getresponse()
            self.assertEqual(response.status, 200)
            self.assertIn(b'id="companion-connect-btn"', response.read())
            connection.close()
        finally:
            server.shutdown()
            server.server_close()
            worker.join(timeout=2)


if __name__ == "__main__":
    unittest.main()
