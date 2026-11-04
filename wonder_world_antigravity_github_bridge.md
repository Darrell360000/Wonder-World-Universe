# WONDER WORLD UNIVERSE // UNIFIED ANTIGRAVITY + GITHUB COPILOT INSTRUCTIONS
**Save Locations in Your Project:**
1. `.github/copilot-instructions.md` *(GitHub Copilot automatically reads this file on every prompt!)*
2. `ANTIGRAVITY.md` or `README.md` *(Antigravity Desktop AI reads this file in your project root!)*

## 1. HOW ANTIGRAVITY & GITHUB COPILOT WORK TOGETHER IN THIS WORKSPACE
You are the AI Engineering Agent operating inside **Antigravity Desktop IDE** alongside **GitHub Copilot** and **GitHub Actions**. Always keep the local Windows 11 workspace and the remote GitHub repository synchronized using this exact division of labor:

| Step | Tool / Agent | What It Does in This Project |
| :--- | :--- | :--- |
| **Step 1: Local Code & CLI Execution** | **Antigravity (Desktop Agent)** | Reads this file, writes/edits Python/C++/C# files on the Windows 11 hard drive, and runs terminal commands (`blender --background`, `dotnet build`, `Build.bat`). |
| **Step 2: Autocomplete & Rule Enforcement** | **GitHub Copilot** | Automatically reads `.github/copilot-instructions.md` to autocomplete Unreal Engine 5 C++, Unity 6 / Cities: Skylines II C#, and Blender 4.x Python code matching Darrell X-Gen's exact design. |
| **Step 3: Cloud Backup, CI/CD & Mobile Hosting** | **GitHub (Git + Actions + Pages)** | Stores all source code via `git push`, runs `.github/workflows/wonder_world_pipeline.yml` in the cloud to build `.fbx`/`.glb` artifacts, and hosts `index.html` live over HTTPS via **GitHub Pages** for the Samsung Galaxy S23 Ultra. |

## 2. CANONICAL CHARACTER SPEC: DARRELL X-GEN (`SK_DarrellXGen` / `BP_DarrellXGen`)
Whenever generating or updating the lead Top-Hatter character in **Blender (`build_darrell_xgen_to_ue5.py`)**, **Unreal Engine 5**, **Unity 6**, or the **WebGL (`index.html`)** game, strictly enforce Darrell's exact likeness from his reference photo:
* **Headwear (`SM_Darrell_TopHat_VelvetCrown` & `SM_Darrell_TopHat_RainbowLED_EQ`):** Matte black velvet Top Hat with a curved brim and a **5-stop Rainbow-Spectrum Pixelated LED Audio Equalizer Band** (`Blue` -> `Green` -> `Yellow` -> `Orange` -> `Red`, Emission Strength `5.2`) that pulses to the **85 BPM** sub-bass beat.
* **Facial Likeness (`SM_Darrell_Head` & `SM_Darrell_MustacheGoatee`):** African American male hero with warm Subsurface Scattering (`0.40` SSS weight), confident expression, and sculpted mustache and goatee.
* **Attire (`SM_Darrell_PinstripeVest` & `SM_Darrell_GreyCollaredShirt`):** Tailored dark charcoal pinstripe vest and matching pinstripe trousers worn over a slate-grey collared long-sleeve dress shirt and polished black dress boots.
* **Cybernetic Weapon (`SM_Darrell_GodHand_Gauntlet`):** Glowing cyan-and-gold **God Hand Sonic Gauntlet** mounted on the forearm (`4.8` emission strength) that channels 60Hz/432Hz/880Hz cymatic shockwaves and executes 0.35s parries.

## 3. CORE UNIVERSE SYSTEMS TO MAINTAIN ACROSS ALL FILES
1. **The 3-Column Financial Ledger (`UWonderLedgerSubsystem` / `WonderTowerEconomyData`):**
   * **Col 1 (Grind / Gross Income):** Passive wages per second from mobilized workforce specialists:
     * **⚡ Speedster Hero:** `+$50.0/s` (dashes at 3x speed)
     * **🎵 Sound Magic DJ:** `+$45.0/s` (phases/vanishes into soundwaves every 3s)
     * **🦺 Construction Builder:** `+$40.0/s` (stacks 1–6 glowing bricks; crowning stabilizes foundation)
     * **🩺 CNA Caregiver:** `+$35.0/s` (dashes between Clinic Door A and Door B)
   * **Col 2 (Overhead / Burn Rate):** Municipal utilities and maintenance (`-$5.0/s` baseline).
   * **Col 3 (Foundation / Net Capital):** `Col 1 - Col 2`. Spendable cash for the **Custom Hat Shop** (**Magnetic Homing Hat $150**, **Golden 2x Multiplier Hat $300**, **Triple-Crown Spread Cannon $500**) and constructing the 6 floors of **Mama Tower** (Cyan/Magenta Bio-Dome Spire) and **Papa Tower** (Obsidian/Gold Financial Spire).
2. **Infinite Doorway Transit Loop (Door A -> Street -> Door B):**
   * Uncrowned citizens continuously spawn from **Door A (Left Arrival Portal)** every 2.4 seconds.
   * When hit by a launched mini hat from the **Mother Wonder Hat Cannon**, they transform into their specialist uniform, march into **Door B (Right Tower Reporting Entrance)**, deposit a **+$25.00 Reporting Bonus**, and trigger a fresh citizen at Door A.
3. **Mini-Me Tophatter Referee & Stage Warp Portal:**
   * Floating robotic companion with golden optics and a mini LED equalizer top hat.
   * Tracks **Hats Gained (Hits)** vs. **Hats Missed** and **Accuracy %**, awards a **+$250 Wonder Reward Chest** every 6 hits, and opens the **Stage 2 Warp Portal** to fight the **Overhead Debt Titan** on the Sky-Deck.

## 4. EXACT TERMINAL COMMANDS FOR ANTIGRAVITY & GITHUB COPILOT
When the user asks you to "build Darrell X-Gen", "run the pipeline", or "sync to GitHub", execute these exact commands in the Antigravity terminal:

### A. Initialize the Repo & Wire Up GitHub Copilot Instructions
```powershell
New-Item -ItemType Directory -Path ".github/workflows", "Scripts", "Assets/WonderWorld/Characters/DarrellXGen" -Force | Out-Null
Copy-Item -Path "Wonder_World_Antigravity_GitHub_Bridge.md" -Destination ".github/copilot-instructions.md" -Force
Copy-Item -Path "Wonder_World_Antigravity_GitHub_Bridge.md" -Destination "README.md" -Force
```

### B. Build Darrell X-Gen in Blender 4.x & Export UE5 FBX / GLB
```powershell
blender --background --python build_darrell_xgen_to_ue5.py -- --output Assets/WonderWorld/Characters/DarrellXGen
```

### C. Build Unity 6 / Cities: Skylines II CLR Mod & Validate UE5 C++
```powershell
if (Test-Path "Scripts/validate_municipal_subsystems.py") { python Scripts/validate_municipal_subsystems.py }
if (Test-Path "WonderWorldUnity/WonderWorldValidation.csproj") { dotnet build WonderWorldUnity/WonderWorldValidation.csproj --configuration Release }
```

### D. Commit & Push Everything to GitHub (Auto-Triggers CI/CD & GitHub Pages)
```powershell
git add -A
git commit -m "Sync Darrell X-Gen Blender-to-UE5 asset & Wonder World Universe via Antigravity + Copilot"
git push -u origin main
```
