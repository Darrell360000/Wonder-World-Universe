# Wonder Command Companion

The Python companion serves the bundled Wonder Command dashboard, synchronizes task lists between a computer and a phone browser, and keeps an activity log in a local SQLite database. It uses only the Python standard library.

## Start on this computer

From the Unreal project folder:

```powershell
py Companion\wonder_command_companion.py
```

Open `http://127.0.0.1:8765/`. The terminal prints a one-time pairing token; choose **Connect companion** in the dashboard and enter that token. The database is stored under `%LOCALAPPDATA%\WonderCommandCompanion\wonder_command.sqlite3` by default.

## Use a phone on the same trusted Wi-Fi

Start the companion with an explicit LAN bind:

```powershell
py Companion\wonder_command_companion.py --host 0.0.0.0
```

Use the computer's private LAN address and port `8765` on the phone. A minimal pairing page asks for the token before it returns the dashboard. Allow the port through Windows Firewall only for a trusted private network. The built-in LAN mode is plain HTTP: do not use it on public Wi-Fi or forward the port from your router. For access outside the home network, use a VPN such as Tailscale/WireGuard with HTTPS rather than exposing this server directly.

## Trust and privacy boundaries

- The default bind is loopback-only. LAN access is an explicit opt-in, serves a token-only pairing page, and uses a random bearer token for dashboard and API access.
- The identity category is not shown to remote devices and cannot be sent to the agent queue. The app rejects strings shaped like Social Security numbers. Do not enter real SSNs, health records, account credentials, or legal case details into prompts.
- Mama/Papa chat sends only the message you type to Google Gemini, and only after you check the explicit data-sharing consent box. The browser voice option uses the device's speech engine; microphone transcription depends on browser support and permissions. Chat text is not stored in the companion logs.
- Gemini can suggest only a task, engine scaffold, or named starter script. Suggestions are validated, staged for approval, and do not change files until you approve them. File copy/move operations are separately staged and cannot overwrite a destination.
- The engine hub creates a shared pipeline layout and starter templates for Blender, Unreal Engine 5.8, Unity, and a Cities: Skylines II mod. It does not control or launch those applications, and templates are not a substitute for version-matched SDK setup or testing.
- Paperwork worksheets are drafts for your review, not official agency forms, legal advice, or submissions. Print / Save as PDF uses the browser print dialog. JSON, XML, Markdown, and CSV downloads are unencrypted; review each export before sharing.
- File browsing is limited to the configured project folder. `Saved`, `Intermediate`, `DerivedDataCache`, and hidden VCS folders are excluded. Copy/move requests are staged, never overwrite, and only run after a user explicitly approves them in the companion.
- The SQLite database is local but not encrypted by this app. Use Windows device encryption and a private Windows account; do not put sensitive records in the companion.
- Activity exports contain event metadata, not prompt bodies. Export them from the dashboard's activity section.

## Optional Gemini chat

The companion runs without an AI key. To enable Mama/Papa chat, obtain a Gemini API key through Google's official tooling and set it in the computer's environment before starting the companion. For a temporary PowerShell session:

```powershell
$env:GEMINI_API_KEY = "your-key"
py Companion\wonder_command_companion.py
```

Do not paste the key into the dashboard, source files, task descriptions, or chat. The key stays on the computer and the user still has to opt in separately for each chat message. Without a configured key, the dashboard clearly reports that Gemini is offline and the message was not sent.

## Engine pipeline and paperwork

Use the dashboard's **Blender + UE5.8 + Unity + Cities II** tab to stage pipeline scaffolding or a catalogued starter script. Review the action description in **Pending actions** and approve it before the companion writes files under `WonderPipeline` inside the configured project folder. The separate engine folders share only the documented common assets and ledger; actual engine projects remain independently configured.

The **Government Paperwork & File Converter** tab generates editable worksheets and downloads JSON, XML, Markdown, or CSV. For a PDF, choose **Print / Save as PDF** and use the browser's print dialog. Templates are generic and start blank so personal medical, identity, and case information is not prefilled into the project.

You can change the project scope and data file:

```powershell
py Companion\wonder_command_companion.py --project-root "D:\MyGame" --database "D:\WonderData\wonder_command.sqlite3"
```

Run another isolated companion for a different engine/project by choosing a separate port, project root, and database:

```powershell
py Companion\wonder_command_companion.py --project-root "D:\Unity\MyGame" --port 8766 --database "D:\WonderData\unity.sqlite3"
```

Run the focused tests with:

```powershell
py -m unittest discover -s Companion\tests -v
```
