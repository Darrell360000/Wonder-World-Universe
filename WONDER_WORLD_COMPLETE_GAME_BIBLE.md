# Mama's Wonder World: Tower Tycoon & Industrial Brawler — Complete Master Game Bible & Step-by-Step Gemini Build Guide

Use this master document whenever you start a new chat with **Gemini**, **Google AI Studio**, **GitHub Copilot**, or your local **Wonder Command Companion (`wonder_command_companion.py`)**. It contains **every single lore detail, character profile, hat power, boss phase, 3-Column Financial Ledger rule, storyboard, Veo 3.1 prompt, and multi-engine code script** from all of your project files so Gemini immediately knows 100% of what is in your game and how to build it step by step.

---

## 1. Copy-Paste Master Context Prompt for Gemini

Copy the entire block below and paste it into **any Gemini chat** (or set it as your **System Instruction** in Google AI Studio / `wonder_command_companion.py`). Once you paste this, Gemini will know every detail of your game without you having to re-explain anything:

```text
SYSTEM DIRECTIVE FOR GEMINI — PROJECT: MAMA'S WONDER WORLD: TOWER TYCOON & INDUSTRIAL BRAWLER

You are the Lead Game Architect, C++/C#/Python Engine Programmer, and Creative Director assisting Darrell Lewis ("Darrell X-Gen") in building "Mama's Wonder World: Tower Tycoon & Industrial Brawler" (also known as "Mama's Universe" / Unreal Engine 5.8 Project: `MamaTowerWonderTycoon.uproject` located at `C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon\`).

1. HARDWARE & LOCAL AI ENVIRONMENT:
- Developer Rig: AMD Ryzen 5 5500 CPU, 32 GB System RAM, NVIDIA GeForce RTX 5060 GPU.
- Local 3D AI Asset Pipeline (100% Offline, $0 Cloud Fees, No Corporate Filters): ComfyUI + Microsoft TRELLIS 2 (`TRELLIS-image-large` fp16) and Hunyuan3D / Modly -> exports `.OBJ` / `.GLB` -> polished in Blender 4.x -> exported to Unreal Engine 5.8 and Unity 2022.3 LTS.
- Companion & Voice Control: Controlled via `Companion\wonder_command_companion.py` (`http://127.0.0.1:8765` with Android phone Wi-Fi pairing) featuring dual AI Co-Op guides:
  * Mama AI ("Big Mama"): Warm, protective, soulful matriarch strategist (`pitch: 0.92, rate: 0.95`) who oversees health pacing, district harmony, and Column 2 Overhead survival.
  * Papa AI ("George Jefferson"): Bold, fast-talking executive builder (`pitch: 1.25, rate: 1.15`) focused on "movin' on up," Column 3 Wonder Capital reserves, and multi-engine shipping.

2. CORE GAME CONCEPT & HYBRID GENRE:
- Genre: Hybrid 2.5D Street Beat-'Em-Up Arcade Brawler + God-Mode Vertical Tower Tycoon / Macro City Builder (with a synchronized Cities: Skylines II custom mod).
- Core Gameplay Loop:
  a) Street-Level 2.5D Brawler: Players control a squad of 5 hot-swappable Top-Hatter Heroes led by Darrell X-Gen (and his shoulder sidekick Minime Tophatter). Combat is powered by "Sound Magic" synced to an 85 BPM boom-bap hip-hop / synth sub-bass rhythm. Players clear "district roughness," purify the Copycat faction, dismantle the Dissonance / Corporate Static Syndicate, defeat multi-phase bosses (like The Syndicate Overlord), and discover Multiverse Portal Keys hidden inside glowing arcade cabinets in the Retro-Futuristic Arcade Lounge & Luxury Mall.
  b) The 3-Column Financial Ledger Economy (`WonderLedgerState.json`):
     - Column 1 (The Grind / Gross Revenue): CNA shifts, wet-vac mini-games, electrical grid balancing, arcade brawler payouts, commercial lease income, and Cities: Skylines II tourism bonuses generate Tier 1 Vitality Credits.
     - Column 2 (The Overhead / Survival Expenses): Rent, healthcare co-pays, union dues, structural maintenance, security/brawler staff, and utility grid load costs must be balanced first before expansion is unlocked.
     - Column 3 (The Foundation / Wonder Capital): Converts net surplus into Tier 2 & Tier 3 Industrial Bonds to construct and upgrade the central megastructure, Mama Wonder Tower.
  c) Macro God-Mode Tower Tycoon: Using the Evon Hub Gauntlet Interface ("PERMISSION GRANTED") and The Divine Hands ("God's Hands Hat"), players snap modular tower blocks onto the city grid, linking Subterranean Basements, Mid-Level Streets, and Upper Towers with glass-domed interactive garden biomes, cyan/magenta "SOUND MAGIC" LED equalizers, holographic Chinese characters, and Wonder Wardens patrols across 5 Generational Eras:
     - Era 1: Concrete Grind -> Era 2: Cloud Grid -> Era 3: Orbital Ring -> Era 4: Lunar Anchor -> Era 5: Purple City (`PROJECT PURPLE CITY`).

3. THE PLAYABLE TOP-HATTER HERO ROSTER (5 SLOTS + EXTENDED LORE VARIANTS):
- Slot 1: Darrell X-Gen ("The Master of Sound Magic" / "The Sonic Tactician") — Primary protagonist (`ADarrellXGenCharacter` / `BP_DarrellXGen`). Wears a metallic Equalizer X-Hat with an animated cyan LED band reading "THE TOP HAT OF MAGICAL SOUND" and "SOUND MAGIC", sleek blue cyber-leather jacket (or Red Jacket cutscene variant / Tailored Grey Suit Vest formal variant), "DARRELL X-GEN / PROJECT PURPLE CITY" graphic tee, white tactical cargo pants, and purple-and-white high-top sneakers. Accompanied by Minime Tophatter ("Mini-Me Wonder Hat") on his shoulder/boots. Wields a conductor's baton electric wand and the Evon Hub Gauntlet ("PERMISSION GRANTED"). Abilities: Frequency Shockwave (`CastSonicShockwave`), Tempo Lock (Matrix-style time slow), Resonance Beam, and Wonder Hat Boomerang finisher (`UnleashWonderHatBoomerang` at 100 Rhythm Heat).
- Slot 2: The Female Top-Hatter Hero ("The Spider-Pulse Icon") — Inspired by Darrell's wife, Micca. High-mobility acrobatic scout wearing a sleek pink-and-white hero suit (plus a Sparkling Black-and-Silver Evening Dress formal city trailer variant), cybernetic forearm gauntlets, and a theatrical top hat with a vibrant pink band. Abilities: Urban web-slinging momentum, wall traversal, and rhythmic audio-pulse energy waves.
- Slot 3: The Divine Hands Top-Hatter ("God's Hands Hat" / Reality Shaper) — Celestial warden in a white-and-gold coat crowned by a celestial top hat with floating, translucent ethereal hands above the brim. Abilities: Colossal God Hands snap modular tower blocks onto the macro-grid, align golden zoning lines across basements/streets/upper towers, catch boss projectiles in mid-air, heal allies in co-op, and rewrite digital code.
- Slot 4: Jaxson "Iron Riff" Thorne ("The Heavy Industrial Brawler") — Tank bruiser in a rain-soaked industrial shipyard aesthetic. Wears a dark gray iron-plated "Anvil Crown" Top Hat bolted with visible hydraulic shock absorbers and orange/yellow warning lights, reinforced tactical vest, and pneumatic sonic-gauntlet fists. Abilities: Seismic Stomp (juggle launch), Kinetic Plating (projectile-reflecting barrier), and Bass-Cannon Smash (sub-bass boss armor shatter).
- Slot 5: The 5th Top-Hatter Specialist (3 Interchangeable Specialist Variants):
  * Variant 5A: The Dimension-Rift Maestro — Wears a shifting trench coat and a holographic crown-shaped top hat projecting purple/blue portal rifts for spatial distortion, wall-bypass rifts, squad teleportation, and pulling arcade portal keys from the ether.
  * Variant 5B: Vesper Vance ("The Cyber-Shadow Infiltrator") — Wears The Phantom Visi-Hat (dark chrome top hat with holographic visor and optical camouflage invisibility). Abilities: Data-Stream Dash (holographic exploding decoy), Shadow Overclock, and Code-Slice Execution (required for Phase 2 of the Syndicate Overlord boss fight).
  * Variant 5C: Tao X Zen ("The Wonder Hat") — Enigmatic mystic in a midnight-blue circuitry suit and a wizard-style theatrical top hat banded with glowing pink-and-gold Taoist sigils.

4. MULTI-ENGINE SEPARATED-TO-UNIFIED PIPELINE (`WonderPipeline\`):
- `01_Blender_Source`: Blender 4.x source `.blend` files + `wonder_blender_ai_addon.py` (Auto-LOD0/1/2 + `UCX_` collision hulls + dual FBX export) + `pack_orm_textures.py` (Red=AO, Green=Roughness, Blue=Metallic `_ORM.png` channel packer for RTX 5060).
- `02_Shared_Game_Core`: Shared `State/WonderLedgerState.json`, `Exports_UE58_FBX/` (`-Y Forward, Z Up`), `Exports_Unity_FBX/` (`-Z Forward, Y Up`), and `Textures_2K_4K_ORM/`.
- `03_UE58_WonderWorld`: Unreal Engine 5.8 (`MamaTowerWonderTycoon.uproject`) with built-in Model Context Protocol (MCP) server on Port 8000 (`ModelContextProtocol.StartServer`), `Scripts\blockout_builder.py`, and C++ `ADarrellXGenCharacter`.
- `04_Unity_Brawler_CS2`: Unity 2022.3 LTS `HeroManager.cs` (keys 1-5 hot-swap) + Cities: Skylines II DOTS/ECS mod `MamaTowerCities2System.cs` (`8m x 8m` zone footprint, `4096x4096` `_ControlMask` and `_Emissive` textures).

Whenever I ask you to build a feature, write code, generate a prompt, or troubleshoot an asset, always keep it 100% consistent with this Game Bible and my local folder structure.
```

---

## 2. Complete World Lore, Setting & The 5 Generational Eras

### 2.1. The World of *Mama's Universe*
*Mama's Wonder World: Tower Tycoon & Industrial Brawler* takes place in a living, vertical cyberpunk metropolis where **architecture and street harmony are governed by acoustic frequency ("Sound Magic")**.
- **The Problem (Static & Roughness)**: Rival corporate syndicates (**The Dissonance / Static Syndicate**) and street-level mimics (**The Copycat Faction**) flood city blocks with chaotic static, oppressive overhead costs, automated laser lockdowns, and structural decay ("district roughness").
- **The Solution (The Wonder Tower & Top-Hatter Wardens)**: Guided by **Mama AI ("Big Mama")** and **Papa AI ("George Jefferson")**, **Darrell X-Gen** and his squad of **Top-Hatter Wardens** take back the streets at **85 BPM**, balance the **3-Column Financial Ledger**, activate the **Evon Hub Gauntlet ("PERMISSION GRANTED")**, and raise **Mama Wonder Tower** block by block from subterranean basements all the way to **Purple City** in deep space.

### 2.2. Key World Landmarks & Vertical Strata
| Landmark / Stratum | Visual & Architectural Details | Gameplay Function |
| :--- | :--- | :--- |
| **Mama Wonder Tower (Central Megastructure)** | Towering cyberpunk spire wrapped in glowing cyan and magenta neon lights, dynamic `"SOUND MAGIC"` LED equalizer patterns, holographic Chinese characters, and massive glass-domed ecosystems. | Central tycoon building that grows across 5 Eras as Column 3 Net Reserve converts into Tier 2 & Tier 3 Industrial Bonds. |
| **Three Vertical Tower Strata** | **1. Subterranean Basements** (utility grid & foundation light-codes)<br>**2. Mid-Level Streets** (urban canyon brawler stages)<br>**3. Upper Towers** (sky-high luxury & garden domes) | Connected into a single seamless loop when **Minime Tophatter** locks foundation light-codes and **The Divine Hands** snap modular blocks into place. |
| **Interactive Glass-Domed Biomes** | Giant glowing biodome garden levels integrated directly into the upper strata of the Wonder Tower. | Boost city attractiveness (`+35% Attractiveness / +20% Tax Yield` in *Cities: Skylines II* sync) and generate passive Vitality Credits. |
| **Retro-Futuristic Arcade Lounge & Luxury Mall** | Multi-level night shopping mall with towering glass atrium ceilings looking out at the rainy neon skyline, polished chrome floors, and rows of cyan/magenta arcade cabinets. | Houses the **Solitary Mystical Arcade Cabinet** pulsing with ethereal energy where players unlock **Multiverse Portal Keys** and launch brawler stages. |
| **Rain-Slicked Industrial Shipyard** | Cluttered night dockyard with towering steel cranes, shipping containers, steel-grated walkways, harsh work lights, and deep puddle reflections. | Signature home turf of **Jaxson "Iron Riff" Thorne** and heavy industrial boss showdowns. |

### 2.3. Progression Through the 5 Generational Eras
1. **Era 1: Concrete Grind**
   - **Setting**: Rain-slicked street-level alleys, industrial shipyards, and subterranean basement foundations.
   - **Focus**: Grinding Tier 1 Vitality Credits through CNA shifts, wet-vac mini-games, electrical balancing, and street brawls against Copycat gangs to stabilize Column 2 Overhead.
2. **Era 2: Cloud Grid**
   - **Setting**: Mid-to-upper skyscraper levels piercing the storm clouds, connected by elevated multi-tiered highways and holographic data streams.
   - **Focus**: Activating the **Evon Hub ("PERMISSION GRANTED")**, unlocking glass-domed interactive garden biomes, and scaling commercial lease income.
3. **Era 3: Orbital Ring**
   - **Setting**: High-atmosphere orbital ring infrastructure surrounding the upper spire of Mama Wonder Tower.
   - **Focus**: Issuing Tier 2 & Tier 3 Industrial Bonds, deploying **Wonder Wardens** patrol squads city-wide, and linking macro utility grids.
4. **Era 4: Lunar Anchor**
   - **Setting**: Cislunar tether anchoring the vertical metropolis to a lunar counterweight station.
   - **Focus**: High-tier multiverse portal expeditions and heavy mech boss defense.
5. **Era 5: Purple City (`PROJECT PURPLE CITY`)**
   - **Setting**: The transcendent synthwave/cyberpunk endgame metropolis bathed in royal purple, cyan, and gold cymatic light.
   - **Focus**: Full generational legacy unlocked — Column 3 Wonder Capital reaches autonomous surplus and the entire city resonates in harmony at 85 BPM.

---

## 3. Complete Character, Companion & AI Guide Bible

### 3.1. Playable Top-Hatter Heroes (The 5-Hero Squad)

| Slot | Hero Name & Title | Signature Hat | Primary Outfit & Gear | Core Combat Style & Signature Abilities |
| :--- | :--- | :--- | :--- | :--- |
| **1** | **Darrell X-Gen**<br>*(The Master of Sound Magic / The Sonic Tactician)* | **Equalizer X-Hat** (`THE TOP HAT OF MAGICAL SOUND`): Sleek black metallic top hat with a real-time animated LED equalizer band displaying `"SOUND MAGIC"`. | **Primary**: Deep blue cyber-leather jacket with circuit stitching, `"DARRELL X-GEN / PROJECT PURPLE CITY"` tee, white tactical cargo pants, purple/white high-top sneakers.<br>**Alternates**: Red Jacket cutscene variant; Tailored Grey Suit Vest formal variant.<br>**Weapons**: Electric Conductor's Baton Wand + Evon Hub Gauntlet. | **Ranged/Brawler Hybrid & Crowd Control (`BP_DarrellXGen`)**:<br>• **Frequency Shockwave**: Low-end bass drop from hat/wand; AoE radial knockback that shatters high-frequency boss shields & clears district roughness.<br>• **Tempo Lock**: Rhythmic pulse that slows time (Matrix-style) around foes.<br>• **Resonance Beam**: Pierces multi-layered shields & purifies Copycats.<br>• **Wonder Hat Boomerang**: Ultimate 360° screen-clearing audio finisher at `100.0` Rhythm Heat. |
| **Companion** | **Minime Tophatter**<br>*(Mini-Me Wonder Hat)* | Miniature digital top hat reading `"Darrell X Gen"` with synced LED equalizer bars. | Matching miniature jacket/cargo outfit and a tiny conductor's baton/cane; perches on Darrell's shoulder or stands between his boots. | **Tactical Advisor & Grid Code Locker**:<br>• Mirrors Darrell's sonic spellcasting in combat.<br>• Taps his tiny wand on street pavement to send geometric light-code ripples into the city's foundation prior to tower construction. |
| **2** | **The Female Top-Hatter Hero**<br>*(The Spider-Pulse Icon)* | **Spider-Pulse Theatrical Top Hat**: Sleek black theatrical top hat accented with a vibrant neon-pink ribbon band (or LED equalizer band in formal mode). | **Primary**: High-mobility pink-and-white superheroine suit with detailed cybernetic forearm gauntlets.<br>**Alternate**: Sparkling black-and-silver evening dress (City Trailer variant).<br>*(Inspired by Darrell's wife, Micca).* | **Acrobatic Scout & Urban Web-Slinger**:<br>• High-speed rooftop acrobatics, wall traversal, and rhythmic pulse-web swinging.<br>• Channels crackling sparks of rhythmic audio-pulse energy from her cybernetic gauntlets to clear street-level syndicates. |
| **3** | **The Divine Hands Top-Hatter**<br>*("God's Hands Hat" / Reality Shaper)* | **God's Hands Celestial Hat**: Transcendent top hat crowned by glowing, translucent ethereal hands floating gracefully above the brim. | Magnificent white-and-gold celestial coat surrounded by floating golden runes and arcs of divine light. | **Macro-Grid Reality Shaper & Co-Op Support**:<br>• Summons colossal translucent **God Hands** from the sky to lift and snap modular tower blocks onto the macro-grid.<br>• Catches heavy boss projectiles in mid-air.<br>• Heals teammates in co-op brawler mode and rewrites digital code in Matrix-style hacking sequences. |
| **4** | **Jaxson "Iron Riff" Thorne**<br>*(The Heavy Industrial Brawler)* | **The Anvil Crown Top Hat**: Heavy-duty dark gray iron-plated top hat bolted with visible hydraulic shock absorbers and glowing orange/yellow warning lights. | Thick reinforced tactical vest over a dark industrial work shirt, heavy-duty steel boots kicking up yellow/orange sparks, and massive pneumatic sonic-gauntlet fists. | **Tank & Vanguard Melee Bruiser**:<br>• **Seismic Stomp**: Slams ground with shockwave gauntlets, launching street enemies into the air for juggle combos.<br>• **Kinetic Plating**: Front-facing kinetic barrier through his Super Magic Coat that reflects bullets/missiles.<br>• **Bass-Cannon Smash**: Industrial sub-bass ultimate punch that shatters boss mech armor in one blow. |
| **5A** | **The Dimension-Rift Maestro**<br>*(5th Slot — Spatial Specialist)* | **Holographic Rift-Crown Hat**: Shifting holographic crown-shaped top hat that continuously projects glowing purple and blue portal rifts. | Dark, shifting cyberpunk trench coat surrounded by swirling cosmic dust and digital distortion. | **Spatial Distortion & Portal Master**:<br>• Opens dimensional rifts to bypass locked walls.<br>• Teleports the Top-Hatter squad across street-level brawler maps.<br>• Pulls multiverse arcade portal keys straight out of the ether. |
| **5B** | **Vesper Vance**<br>*(5th Slot Alt — The Cyber-Shadow Infiltrator)* | **The Phantom Visi-Hat**: Dark chrome top hat with built-in holographic projection visor and optical camouflage nodes for sound-stealth invisibility. | High-mobility stealth cyber-suit with code-slicing blades. | **Stealth & Security Grid Saboteur**:<br>• **Data-Stream Dash**: Dashes in digital code, leaving a detonating holographic decoy.<br>• **Shadow Overclock**: Boosts attack speed & crit rate when dodging laser grids.<br>• **Code-Slice Execution**: Aerial diving strike that instantly disables corporate drones & mainframes. |
| **5C** | **Tao X Zen**<br>*(5th Slot Alt — The Wonder Hat Mystic)* | **Taoist Sigil Wizard Top Hat**: Custom dark felt theatrical wizard-style top hat with a wide brim and tall pointed crown, banded with glowing pink-and-gold Taoist sigils. | Reflective midnight-blue hero suit with glowing silver and cyan circuitry patterns and futuristic cybernetic forearm gauntlets. | **Arcane Audio-Pulse Mystic**:<br>• Fires spiraling arcane light and pulsating sound waves from an open-palm crouch stance.<br>• Harmonizes spiritual chi with digital street frequencies. |

### 3.2. Co-Op AI Guides (Wonder Command Voice Agents)
- **👩🏽 Mama AI ("Big Mama")**:
  - **Voice Profile**: Warm, steady, protective, soulful matriarch (`pitch: 0.92`, `rate: 0.95`).
  - **Role in Game & Companion**: Watches over player stamina, medical/fatigue pacing, district community wellness, and **Column 2 (The Overhead)** survival bills so the household and tower foundation never collapse.
- **🧔🏽‍♂️ Papa AI ("George Jefferson")**:
  - **Voice Profile**: Sharp, ambitious, fast-talking executive builder (`pitch: 1.25`, `rate: 1.15`).
  - **Role in Game & Companion**: Drives upward mobility (*"movin' on up!"*), manages **Column 3 (The Foundation / Wonder Capital)** bond conversions, dispatches Unreal/Unity engine build commands, and maximizes commercial lease & tourism yields.

---

## 4. Enemy Factions & Multi-Phase Boss Encounters

### 4.1. Enemy Factions
1. **The Copycat Faction**: Street-level impostors and static clones that mimic the Top-Hatters' style without the soul or frequency. Darrell X-Gen's **Resonance Beam** and **Frequency Shockwave** purify Copycat members and restore street harmony.
2. **The Dissonance / Corporate Static Syndicate**: High-tech corporate enforcers, automated security turrets, tracking laser grids, and corporate drone swarms that lock down city blocks and drain Column 2 Overhead.

### 4.2. Flagship Boss Fight: "THE SYNDICATE OVERLORD" (3-Phase Tactical Tag-Team Battle)
Victory against **The Syndicate Overlord** is impossible with a single character — players **must** hot-swap between their Top-Hatter Heroes (keys `1`–`5` in `HeroManager.cs`) across three distinct phases:

- **Phase 1: The Sonic Barrier Breach (Required Hero: Darrell X-Gen)**
  - *Boss State*: The Overlord is encased in an impenetrable high-frequency sound barrier that absorbs all physical attacks and deflects standard projectiles.
  - *Gameplay Mechanic*: Darrell X-Gen takes the field, timing his wand strikes to the **85 BPM** ambient track using his **Equalizer X-Hat**. When `RhythmHeat` peaks, Darrell unleashes **Frequency Shockwave** to overload the boss's audio-shield generators.
  - *Phase Transition*: The equalizer bar maxes out, the sonic shield shatters, and the Overlord drops to one knee before retreating behind heavy security walls — triggering a **Tag-Team Hot-Swap**.
- **Phase 2: The Laser Grid Sabotage (Required Hero: Vesper Vance / Spider-Pulse Infiltrator)**
  - *Boss State*: The arena locks down with automated security turrets, tracking laser grids, and high-speed corporate drone swarms where heavy brawlers cannot easily maneuver.
  - *Gameplay Mechanic*: The player hot-swaps to **Vesper Vance** (or **Spider-Pulse Heroine**). Using **Phantom Visi-Hat** stealth, **Data-Stream Dash**, and **Shadow Overclock**, she bypasses the perimeter laser grid, reaches the central syndicate mainframe terminal, and executes **Code-Slice Execution** to shut down all arena defenses.
- **Phase 3: The Armored Juggernaut Showdown (Required Hero: Jaxson "Iron Riff" Thorne)**
  - *Boss State*: Stripped of his shields and laser grid, the Overlord activates a massive robotic mech-suit, charging across the wet asphalt and firing heavy ballistic missiles.
  - *Gameplay Mechanic*: The player hot-swaps to **Jaxson "Iron Riff" Thorne**. Jaxson activates **Kinetic Plating** to reflect ballistic fire, slams the steel ground with **Seismic Stomp** to interrupt the mech's charge, and delivers a screen-shaking **Bass-Cannon Smash** fueled by his **Anvil Crown Top Hat** to shatter the mech's armor permanently.

---

## 5. The 3-Column Financial Ledger & Mini-Game Economy

At the heart of *Mama's Wonder World: Tower Tycoon* is **The 3-Column Financial Ledger**, which translates real-life hustle and survival discipline into city-building power. Both **Unreal Engine 5.8** and **Unity / Cities: Skylines II** read and write to the same shared state file (`02_Shared_Game_Core/State/WonderLedgerState.json`).

### 5.1. How the 3 Columns Work
| Ledger Column | Name & Core Theme | Active Revenue / Expense Sources & Mini-Games | Currency & Gameplay Output |
| :--- | :--- | :--- | :--- |
| **Column 1** | **The Grind**<br>*(Gross Revenue & Street Hustle)* | • **CNA Shift Mini-Games** (patient care & stamina management)<br>• **Wet-Vac Water Extraction Mini-Games** (clearing 59,000-gallon flood emergencies)<br>• **Electrical Grid Balancing**<br>• **2.5D Arcade Brawler Payouts**<br>• **Commercial Tower Lease Income**<br>• **Cities: Skylines II Tourism Bonus** | Generates **Tier 1 Vitality Credits** and raw gross cash flow (`$71,750.00` baseline in shared ledger state). |
| **Column 2** | **The Overhead**<br>*(Operating Expenses & Survival)* | • **Household Rent & Security Deposit Protection**<br>• **Healthcare Co-Pays & Stamina Recovery**<br>• **Union Dues**<br>• **Tower Structural Maintenance** (`$11,200`)<br>• **Security & Brawler Staff** (`$8,400`)<br>• **Utility Grid Load Cost** (`$5,100`) | **Hard Gate Rule**: Column 2 must be 100% covered and balanced every cycle before the Evon Hub grants `"PERMISSION GRANTED"` for tower expansion. |
| **Column 3** | **The Foundation**<br>*(Net Reserve & Wonder Capital)* | • **Net Surplus Conversion** (`Col 1 - Col 2 = Net Reserve`)<br>• **Tier 2 & Tier 3 Industrial Bonds**<br>• **8m x 8m Cities: Skylines II Lot Expansion**<br>• **4096x4096 Emissive Signage & Biome Unlocks** | Unlocks new **Mama Wonder Tower** floors, glass-domed garden biomes, and advances the city from **Era 1 (Concrete Grind)** to **Era 5 (Purple City)**. |

### 5.2. Shared JSON Schema (`02_Shared_Game_Core/State/WonderLedgerState.json`)
```json
{
  "schema_version": "3.0.0",
  "tower_name": "Mama Wonder Tower",
  "current_era": 1,
  "era_name": "Era 1: Concrete Grind",
  "hardware_profile": "RTX_5060_32GB_RAM",
  "sound_magic_bpm": 85,
  "evon_hub_permission_granted": true,
  "ledger_columns": {
    "column_1_gross_revenue": {
      "cna_and_wetvac_minigames": 8500.00,
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
      "liquid_reserve": 55550.00,
      "vitality_credits_tier1": 12400,
      "industrial_bonds_tier2_3": 85,
      "unlocked_tier": 3,
      "cities_skylines_lot_size": "8m x 8m",
      "emissive_signage_active": true
    }
  }
}
```

---

## 6. Official Cutscene Storyboards & Google Veo 3.1 / SeedDance Video Prompts

### 6.1. Cutscene Storyboard 1: *"The Resonance of the Top Hat"* (4 Panels)
- **Panel 1 — The Awakening**: Close-up at street level. A tiny **Minime Tophatter** stands proudly on the pavement between towering city blocks wearing his miniature digital top hat reading `"Darrell X Gen"`. He taps his small wand on the ground, creating a tiny ripple of glowing musical notes.
  - *Dialogue*: `"Every great frequency starts with a single pulse."`
- **Panel 2 — The Scaling Up**: Dramatic low-angle shot looking up. The energy from Minime's sound waves shoots upward, shifting reality and projecting the full-size **Darrell X-Gen** into the mid-level street district in his blue cyber-leather jacket and glowing white cargo pants, eyes charged with electric white energy.
  - *Dialogue*: `"Scaling up the vibration."`
- **Panel 3 — Sound Magic in Action**: Wide shot of the urban canyon street. Darrell swings his conductor's wand, casting a sweeping wave of neon musical staffs and cymatic frequencies across the buildings to cleanse district roughness and trigger a **Net-Power Overcharge**.
  - *Dialogue*: `"Resonate the blocks. Clear the static."`
- **Panel 4 — The Legacy**: Dual-scale hero shot. Full-size Darrell X-Gen stands tall while Minime Tophatter stands right at his boots in a matching pose as the top hat equalizer pulses, signaling the district is pristine and secured by the **Wonder Wardens**.
  - *Dialogue*: `"Mama's Universe: Where the sound shapes the city."`

### 6.2. Cutscene Storyboard 2: *"The Architect's Symphony"* (4 Panels)
- **Panel 1 — The Wonder Hat Activation**: Close-up on the glowing Wonder Hat pulsing with frequency waves against a moody urban backdrop. (*Caption: "When the frequency shifts, the universe listens."*)
- **Panel 2 — Minime's Groundwork**: Low-angle street view of Minime tapping the pavement with his miniature baton to lock geometric light codes into the city's grid foundation. (*Caption: "Small scale, absolute precision."*)
- **Panel 3 — The God Hands Shaping Reality**: Macro-scale overhead view where colossal, translucent **God Hands** reach down from the upper strata skies, gently lifting and snapping modular tower blocks into place as golden zoning lines connect subterranean basements, mid-level streets, and upper towers. (*Caption: "Macro architecture meets divine mechanics."*)
- **Panel 4 — The Sound Magic Tophatter Takes the Street**: Medium shot of Darrell X-Gen in the center of the wet neon street with floating shards of glass suspended in mid-air, conducting the final symphony that locks in the **Wonder Wardens** patrol squad. (*Caption: "Mama's Universe: Sound shapes the city."*)

### 6.3. Cutscene Storyboard 3: *"The Genesis of the Wonder Tower"* (8-Panel Director's Sequence)
1. **Panel 1 (ECU — The Awakening)**: Static extreme close-up on the top of the Tophatter's glowing digital hat; equalizer bars pulse rapidly with subtle heat-haze distortion.
2. **Panel 2 (CU — The First Beat)**: Slow pull-back revealing the Tophatter (Red Jacket variant, blue glowing hat) on a dark, wet skyscraper rooftop at golden hour holding his conductor's wand.
3. **Panel 3 (ELA — Minime's Mission)**: Extreme low angle between the Tophatter's boots where **Minime Tophatter** taps his miniature wand on the wet concrete, creating a geometric light ripple; camera tilts up slowly.
4. **Panel 4 (MS — The Command)**: Tophatter raises his glowing hand, activating his bracelet interface as the holographic **Evon Hub** panel floats into view above the street below.
5. **Panel 5 (FS — Permission Granted)**: Tophatter swipes his hand and massive golden holographic text reading `"PERMISSION GRANTED"` materializes across the entire city block via a fast whip-pan.
6. **Panel 6 (Low Angle — Tower Activation)**: Vertigo zoom-in on a dark modular construction site in the distance as blue and cyan neon Sound Magic lines surge up the structure, connecting all levels.
7. **Panel 7 (Aerial High Angle — The Build is Complete)**: Slow orbit rotation around the completed **Mama Wonder Tower** rising above the cloud layer, its glass-domed garden biomes glowing against the stormy sunset sky.
8. **Panel 8 (WS — The Wonder Wardens)**: Slow push-in at street level as the Tophatter is joined by his fellow Top-Hatter Wardens standing shoulder-to-shoulder, looking up at the completed tower.

### 6.4. Ready-to-Run Video & Image Generation Prompts (Google Veo 3.1, SeedDance 2.5 & Midjourney/ComfyUI)
- **Google Veo 3.1 — Option 1 (9:16 Vertical Image-to-Video for 4-Hero Lineup)**:
  > `9:16 vertical, low-angle slow push-in tracking shot at 60fps. Four Top-Hatter heroes stand on a rain-slicked cyberpunk street: Darrell X-Gen in the center wearing a neon-trimmed blazer, graphic tee, denim, high-top sneakers, and a glowing "SOUND MAGIC" LED equalizer hat with Mini-Me on his shoulder; flanked by the Spider-Pulse heroine in a pink-and-white suit, the Divine Hands hero with floating ethereal hands above a golden crown, and Jaxson Thorne in a leather tactical jacket, Anvil Crown warning-light hat, and metallic gauntlet. Darrell slashes his glowing purple wand, sending cymatic shockwaves rippling across the wet asphalt as the squad steps forward into combat readiness. Towering neon skyscrapers frame the dusk sky. Photorealistic 8k masterpiece, Unreal Engine 5.8 render, ray-traced mirror reflections on wet pavement, volumetric fog, anamorphic lens flare. Audio: deep industrial sub-bass drop, crackling electric wand hum, rain, and heavy footsteps on wet pavement.`
- **Google Veo 3.1 — Option 2 (16:9 High-Octane Streetwear & Gameplay Trailer)**:
  > `Dynamic low-angle tracking shot transitioning into a slow-motion action portrait, 35mm lens. Darrell X-Gen and his Top-Hatter squad charge down a neon-lit metropolis street wearing luxury cyberpunk streetwear—custom cyber-leather jackets, utility cargo pants, pristine high-top sneakers, and animated LED equalizer top hats. Darrell swings his electric baton, unleashing neon cymatic waves that shatter street static and ignite the massive modular Wonder Tower in the background with cyan and magenta equalizer lights. Photorealistic 8k masterpiece, Unreal Engine 5 cinematic action sequence, ray-traced puddle reflections, volumetric neon lighting. Audio: pulsing hip-hop synth bassline, electric arcs, and cinematic impact booms.`
- **SeedDance 2.5 — City Builder & Formal Top-Hatter Squad Trailer Prompt**:
  > `A cinematic, ultra-detailed game trailer shot for a futuristic city-building game, set in a sprawling cyberpunk metropolis at night with neon lights and flying vehicles. In the foreground, a diverse squad of heroic top-hatter characters stand together: a Black man wearing a tailored grey suit vest and a sleek black top hat featuring a glowing multicolored LED equalizer band, flanked by a powerful Black woman in an elegant sparkling black-and-silver evening dress with a matching LED equalizer top hat, alongside futuristic sci-fi companions. The central character raises their hand, casting purple magic and performing a teleportation spell with a flash of light. Unreal Engine 5 render style, ray-traced reflections, highly detailed character assets, vibrant synthwave lighting, 8k resolution.`

---

## 7. Step-by-Step Instructions for Building the Game with Gemini

Follow these exact 5 stages with Gemini to build, sync, and playtest every part of *Mama's Wonder World: Tower Tycoon & Industrial Brawler* on your PC (`C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon\`).

### Step 1: Scaffold the Separated-to-Unified 4-Folder Architecture
To keep **Blender 4.x**, **Unreal Engine 5.8**, and **Unity 2022.3 LTS / Cities: Skylines II** from ever corrupting each other's `.meta` or `Intermediate` files, keep them in sibling folders inside `C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon\WonderPipeline\`:

```text
C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon\
├── MamaTowerWonderTycoon.uproject
├── Companion\
│   ├── wonder_command_companion.py   <-- Local Python server (Port 8765) + Mama & Papa AI
│   ├── web\index.html                <-- Wonder Command v3.0 UI
│   └── tests\test_companion.py
├── Scripts\
│   └── blockout_builder.py           <-- UE5 Python script to build initial tower blockout
└── WonderPipeline\
    ├── 01_Blender_Source\
    │   ├── Characters_Rigged\
    │   ├── Tower_Modular_Meshes\
    │   └── Scripts\
    │       ├── wonder_blender_ai_addon.py
    │       └── pack_orm_textures.py
    ├── 02_Shared_Game_Core\
    │   ├── State\WonderLedgerState.json
    │   ├── Exports_UE58_FBX\         <-- Axis: -Y Forward, Z Up (1 unit = 1 cm)
    │   ├── Exports_Unity_FBX\        <-- Axis: -Z Forward, Y Up (1 unit = 1 m)
    │   └── Textures_2K_4K_ORM\       <-- Channel-packed _ORM.png textures
    ├── 03_UE58_WonderWorld\
    │   └── Source\WonderWorld\
    │       ├── DarrellXGenCharacter.h
    │       └── DarrellXGenCharacter.cpp
    ├── 04_Unity_Brawler_CS2\
    │   └── Assets\WonderShared\
    │       ├── HeroManager.cs
    │       └── MamaTowerCities2System.cs
    ├── 06_Engine_Command_Logs\
    │   ├── ue58_command_bus.jsonl
    │   └── unity_cs2_command_bus.jsonl
    └── sync_wonder_pipeline.py
```

**What to tell Gemini for Step 1**:
> *"Gemini, generate or update `sync_wonder_pipeline.py` so it verifies all folders in `WonderPipeline\`, checks `WonderLedgerState.json`, and copies any new `.fbx` and `_ORM.png` files from `02_Shared_Game_Core` into both `03_UE58_WonderWorld` and `04_Unity_Brawler_CS2`."*

---

### Step 2: Generate 3D Assets Locally on Your RTX 5060 (ComfyUI + TRELLIS 2 -> Blender 4.x)
Your **NVIDIA RTX 5060 + 32 GB RAM** rig can generate 3D meshes (`OBJ` / `GLB`) completely offline with zero cloud fees and zero filters:

1. **Run ComfyUI + TRELLIS 2 Locally**:
   - Drop your 2D character portrait (Darrell X-Gen, Spider-Pulse, Divine Hands, Jaxson Thorne, or Mama Wonder Tower) into the **ComfyUI Load Image Node**.
   - Connect it to `TRELLIS-image-large (fp16)` -> export as `.GLB` or `.OBJ` into `01_Blender_Source\`.
2. **Run the Wonder Command Blender AI Add-on (`wonder_blender_ai_addon.py`)**:
   - Open **Blender 4.x**, go to `Scripting` -> `Open` -> select `01_Blender_Source\Scripts\wonder_blender_ai_addon.py` and click **Run Script**.
   - Press `N` in the 3D Viewport to open the **Wonder Command AI** sidebar tab:
     - Click **"1. Build LOD0 / LOD1 / LOD2 + UCX Collision"** to automatically decimate high-poly AI meshes into `_LOD0` (100%), `_LOD1` (50%), `_LOD2` (25%) and build an Unreal `UCX_` convex collision hull.
     - Click **"2. Dual Export FBX (UE5.8 + Unity/CS2)"** to automatically export with `-Y Forward, Z Up` for Unreal Engine 5.8 and `-Z Forward, Y Up` for Unity / *Cities: Skylines II*.
3. **Pack PBR Textures for RTX 5060 (`pack_orm_textures.py`)**:
   - Run `py 01_Blender_Source\Scripts\pack_orm_textures.py` to pack Ambient Occlusion (`R`), Roughness (`G`), and Metallic (`B`) into a single `_ORM.png` texture inside `02_Shared_Game_Core\Textures_2K_4K_ORM\`.

---

### Step 3: Build & Connect Unreal Engine 5.8 (`MamaTowerWonderTycoon.uproject`)
1. **Enable the Built-In Unreal Engine 5.8 MCP Server (Port 8000)**:
   - Open `MamaTowerWonderTycoon.uproject` in **Unreal Engine 5.8**.
   - Go to **Edit > Plugins**, search for `Model Context Protocol`, and enable **Unreal MCP** and its toolsets.
   - Open the Unreal Output Log / Console (`~`) and run:
     ```text
     ModelContextProtocol.StartServer
     ModelContextProtocol.GenerateClientConfig All
     ```
   - In your Windows terminal, verify the bridge is live on Port 8000 by running:
     ```powershell
     npx unreal-mcp-cli status
     npx unreal-mcp-cli wait-for-ready
     ```
2. **Run the Tower Blockout Builder (`Scripts\blockout_builder.py`)**:
   - Inside Unreal Engine 5.8 (`Tools > Execute Python Script`), run `Scripts\blockout_builder.py` to spawn the modular blockout of **Mama Wonder Tower** (Subterranean Basement, Mid-Level Street Canyon, and Upper Biome Spire).
3. **Compile Darrell X-Gen's C++ Character Class (`DarrellXGenCharacter.h` / `.cpp`)**:
   - Place `DarrellXGenCharacter.h` and `DarrellXGenCharacter.cpp` inside `Source\WonderWorld\`:
     - Enforces the 2.5D brawler plane constraint (`GetCharacterMovement()->bConstrainToPlane = true`).
     - Implements `UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Magic") float RhythmHeat = 0.0f;`
     - Implements `CastSonicShockwave(FVector TargetLocation)` (`+15.0f` Rhythm Heat per beat hit) and `UnleashWonderHatBoomerang()` (fires at `100.0f` Rhythm Heat).
   - Create Blueprint `BP_DarrellXGen` inheriting from `ADarrellXGenCharacter`, attach the **Equalizer X-Hat** mesh to the `head` socket, the **Minime Tophatter** companion mesh to the `clavicle_l` shoulder socket, and the **Conductor Wand** to `hand_r`.

**What to tell Gemini for Step 3**:
> *"Gemini, write the Unreal Engine 5.8 Blueprint/C++ logic for `ADarrellXGenCharacter` or the 3-Phase Syndicate Overlord Boss Actor (`ASyndicateOverlordBoss`) so Phase 1 requires Darrell's `CastSonicShockwave`, Phase 2 requires Vesper Vance's stealth terminal hack, and Phase 3 requires Jaxson Thorne's `BassCannonSmash`."*

---

### Step 4: Build the Unity 2022.3 LTS Brawler & *Cities: Skylines II* Custom Mod
1. **Set Up the 5-Hero Hot-Swap Manager in Unity (`HeroManager.cs`)**:
   - Open `04_Unity_Brawler_CS2` in **Unity Hub (Unity 2022.3 LTS)**.
   - Attach `HeroManager.cs` to an empty `GameManager` object in your scene.
   - Drag the 5 hero prefabs into the `Heroes` array in the Inspector:
     - `Element 0`: `Darrell_X_Gen`
     - `Element 1`: `Spider_Pulse_Heroine`
     - `Element 2`: `Divine_Hands_TopHatter`
     - `Element 3`: `Jaxson_Iron_Riff_Thorne`
     - `Element 4`: `Dimension_Rift_Maestro` (or `Vesper_Vance` / `Tao_X_Zen`)
   - Press keys `1` through `5` during Play Mode to hot-swap heroes at the exact same street coordinates mid-combo!
2. **Compile the *Cities: Skylines II* Custom Mod (`MamaTowerCities2System.cs`)**:
   - Place `MamaTowerCities2System.cs` in your *Cities: Skylines II* mod project (`04_Unity_Brawler_CS2\Mods_CitiesSkylines2\`).
   - It inherits from `GameSystemBase` (`Unity.Entities` / `Game.Simulation`), reads `02_Shared_Game_Core\State\WonderLedgerState.json` 16 times per in-game day (`GetUpdateInterval = 262144`), enforces the `8m x 8m` zone lot footprint for **Mama Wonder Tower**, and lights up the `4096x4096` `_ControlMask` and `_Emissive` cyan/magenta equalizer windows whenever `emissive_signage_active` is `true` in Column 3.

---

### Step 5: Run Mama AI & Papa AI in Hybrid Mode (`wonder_command_companion.py`)
1. **Start the Companion Server on Your PC**:
   - Open PowerShell in `C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon\` and run:
     ```powershell
     $env:GEMINI_API_KEY="YOUR_GOOGLE_AI_STUDIO_KEY"
     py Companion\wonder_command_companion.py
     ```
2. **How Hybrid Mode Protects Your Workflow**:
   - **When Online (`GEMINI_API_KEY` set)**: `POST /api/agent/chat` calls `gemini-2.5-flash` to generate live, context-aware Co-Op replies from both **Mama AI ("Big Mama")** and **Papa AI ("George Jefferson")**, spoken aloud through your browser's speech synthesizer (`window.speechSynthesis`).
   - **When Offline or Quota Reached**: Automatically falls back to the built-in local Mama & Papa response templates and local SQLite/`.wlog` command bus so your voice controls, task board, and engine scripts never stop working.
   - **Human Approval Gate**: Any file or engine action proposed by Mama or Papa is staged as `pending_approval` until you click **"Approve & Run"** in the Wonder Command UI.

---

## 8. Quick Reference Cheat Sheet of Prompts to Ask Gemini Next

Whenever you open Gemini to work on the game, you can paste Section 1 above and then ask any of these exact prompts:
- **For Unreal Engine 5.8 C++ Combat**: *"Using the Mama's Wonder World Game Bible, write the complete `ASyndicateOverlordBoss.h` and `.cpp` class in Unreal Engine 5.8 for the 3-phase boss fight where Phase 1 requires Darrell X-Gen's `CastSonicShockwave`, Phase 2 requires Vesper Vance's laser grid terminal hack, and Phase 3 requires Jaxson Thorne's `BassCannonSmash`."*
- **For the 3-Column Financial Ledger UI**: *"Write an Unreal Engine 5.8 UMG Widget Blueprint / C++ ViewModel (`UWonderLedgerViewModel`) that reads `02_Shared_Game_Core/State/WonderLedgerState.json`, displays Column 1 (The Grind), Column 2 (The Overhead), and Column 3 (The Foundation), and triggers the 'PERMISSION GRANTED' Evon Hub animation when Column 2 is paid off."*
- **For the Wet-Vac & CNA Grind Mini-Games**: *"Write a playable mini-game script for Era 1: Concrete Grind where completing a Wet-Vac 59,000-gallon water extraction challenge or CNA shift awards Tier 1 Vitality Credits into Column 1 of `WonderLedgerState.json`."*
- **For Blender 4.x Character Rigging**: *"Write a Blender 4.x Python (`bpy`) script that rigs Darrell X-Gen's Equalizer X-Hat and attaches Minime Tophatter to his left shoulder bone (`clavicle.L`) with an idle floating bounce animation."*

---

## 9. Complete 3-Column Financial Ledger & Wet-Vac / CNA Grind Mini-Game Scripts (Unreal Engine 5.8 C++ & Unity C#)

This section provides the complete, production-ready code for **The 3-Column Financial Ledger** and **The Era 1 ("Concrete Grind") Mini-Games**:
1. **The 3-Column Financial Ledger Subsystem (`WonderLedgerSubsystem.h` / `.cpp`)**: Manages **Column 1 (The Grind)**, **Column 2 (The Overhead)**, and **Column 3 (The Foundation / Wonder Capital)**, enforces the **Evon Hub `"PERMISSION GRANTED"`** survival gate, converts net surplus into **Tier 2 & Tier 3 Industrial Bonds**, advances the tower across all **5 Generational Eras**, and syncs directly with `02_Shared_Game_Core/State/WonderLedgerState.json`.
2. **The Wet-Vac 59,000-Gallon Flood Extraction, CNA CareShift & Electrical Grid Mini-Game Component (`WonderGrindMiniGameComponent.h` / `.cpp`)**:
   - **Wet-Vac 59,000-Gallon Basement Flood Extraction**: Players pump out a massive 59,000-gallon basement water leak in **Era 1: Concrete Grind**. Timing suction bursts to the **85 BPM Sound Magic** beat grants a `1.5x` extraction multiplier, awards **Tier 1 Vitality Credits** into Column 1, and clears the subterranean foundation so **Minime Tophatter** can lock in the geometric light-codes.
   - **CNA Shift & Patient Care Triage Mini-Game**: Simulates high-demand CNA floor shifts (Hoyer lift transfers, vital-sign checks, ADL care) paired with **Mama AI's Stamina & Leg-Elevation Pacing Mechanic** (`TakeMamaRestBreak()`), rewarding smart pacing with **Tier 1 Vitality Credits** and **Column 1 Gross Revenue**.
   - **Electrical Grid Load Balancer**: Locks 3-phase breaker sine waves to 85 BPM to slash **Column 2 Utility Grid Load Cost** by 35% and trigger a **Net-Power Overcharge**.
3. **Unity 2022.3 LTS C# All-in-One Ledger & Mini-Game Manager (`WonderLedgerAndGrindController.cs`)**: Keeps your Unity 2.5D Brawler and *Cities: Skylines II* mod 100% synced with the exact same JSON ledger file.

---

### 9.1. Unreal Engine 5.8 C++ — 3-Column Financial Ledger Subsystem (`WonderLedgerSubsystem.h` & `WonderLedgerSubsystem.cpp`)
Save in `03_UE58_WonderWorld\Source\WonderWorld\`:

```cpp
// WonderLedgerSubsystem.h - Unreal Engine 5.8
// Mama's Wonder World: Tower Tycoon & Industrial Brawler
// Manages the 3-Column Financial Ledger, Evon Hub Clearance, and 5 Generational Eras

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WonderLedgerSubsystem.generated.h"

UENUM(BlueprintType)
enum class EWonderEra : uint8
{
    Era1_ConcreteGrind UMETA(DisplayName = "Era 1: Concrete Grind"),
    Era2_CloudGrid     UMETA(DisplayName = "Era 2: Cloud Grid"),
    Era3_OrbitalRing   UMETA(DisplayName = "Era 3: Orbital Ring"),
    Era4_LunarAnchor   UMETA(DisplayName = "Era 4: Lunar Anchor"),
    Era5_PurpleCity    UMETA(DisplayName = "Era 5: Purple City")
};

// COLUMN 1: THE GRIND (Gross Revenue & Street Hustle)
USTRUCT(BlueprintType)
struct FColumn1TheGrind
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 1: The Grind")
    float CNAAndWetVacMiniGames = 8500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 1: The Grind")
    float ArcadeBrawlerPayouts = 14500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 1: The Grind")
    float CommercialLeaseIncome = 48000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 1: The Grind")
    float CitiesSkylinesTourismBonus = 9250.0f;

    float GetTotalGrossRevenue() const
    {
        return CNAAndWetVacMiniGames + ArcadeBrawlerPayouts + CommercialLeaseIncome + CitiesSkylinesTourismBonus;
    }
};

// COLUMN 2: THE OVERHEAD (Operating Expenses & Survival Gate)
USTRUCT(BlueprintType)
struct FColumn2TheOverhead
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 2: The Overhead")
    float RentAndHouseholdSurvival = 2800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 2: The Overhead")
    float HealthcareAndStaminaCare = 1200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 2: The Overhead")
    float StructuralMaintenance = 11200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 2: The Overhead")
    float SecurityAndBrawlerStaff = 8400.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 2: The Overhead")
    float UtilityGridLoadCost = 5100.0f;

    float GetTotalOperatingExpenses() const
    {
        return RentAndHouseholdSurvival + HealthcareAndStaminaCare + StructuralMaintenance + SecurityAndBrawlerStaff + UtilityGridLoadCost;
    }
};

// COLUMN 3: THE FOUNDATION (Net Reserve, Vitality Credits & Industrial Bonds)
USTRUCT(BlueprintType)
struct FColumn3TheFoundation
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 3: The Foundation")
    float LiquidReserve = 51550.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 3: The Foundation")
    int32 VitalityCreditsTier1 = 12400;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 3: The Foundation")
    int32 IndustrialBondsTier2And3 = 85;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 3: The Foundation")
    int32 UnlockedTowerTier = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 3: The Foundation")
    FString CitiesSkylinesLotSize = TEXT("8m x 8m");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Column 3: The Foundation")
    bool bEmissiveSignageActive = true;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWonderLedgerUpdated, float, GrossRevenue, float, TotalOverhead, float, NetReserve);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMamaPapaVoiceAdvisory, FString, MamaLine, FString, PapaLine);

UCLASS(BlueprintType)
class WONDERWORLD_API UWonderLedgerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder Ledger")
    FColumn1TheGrind Column1Grind;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder Ledger")
    FColumn2TheOverhead Column2Overhead;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder Ledger")
    FColumn3TheFoundation Column3Foundation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder Ledger|Progression")
    EWonderEra CurrentEra = EWonderEra::Era1_ConcreteGrind;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wonder Ledger|Evon Hub")
    bool bEvonHubPermissionGranted = true;

    UPROPERTY(BlueprintAssignable, Category = "Wonder Ledger|Events")
    FOnWonderLedgerUpdated OnLedgerUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Wonder Ledger|Events")
    FOnMamaPapaVoiceAdvisory OnMamaPapaVoiceAdvisory;

    // Adds income from CNA shifts, Wet-Vac extraction, or Brawler combos into Column 1
    UFUNCTION(BlueprintCallable, Category = "Wonder Ledger|Actions")
    void RecordGrindIncome(float CashEarned, int32 VitalityCreditsEarned, const FString& SourceLabel);

    // Applies utility/plumbing leak relief to reduce Column 2 Overhead
    UFUNCTION(BlueprintCallable, Category = "Wonder Ledger|Actions")
    void ApplyOverheadRelief(float UtilityReductionAmount);

    // Balances Column 1 vs Column 2 and updates Evon Hub "PERMISSION GRANTED" status
    UFUNCTION(BlueprintCallable, Category = "Wonder Ledger|Actions")
    bool AuditAndBalanceLedger();

    // Converts Column 3 Net Reserve into Tier 2/3 Industrial Bonds to build Mama Wonder Tower
    UFUNCTION(BlueprintCallable, Category = "Wonder Ledger|Actions")
    bool ConvertSurplusToIndustrialBonds(float ReserveAmountToConvert);

    // Saves shared state to 02_Shared_Game_Core/State/WonderLedgerState.json
    UFUNCTION(BlueprintCallable, Category = "Wonder Ledger|Sync")
    bool SaveSharedLedgerJson();

    UFUNCTION(BlueprintPure, Category = "Wonder Ledger|Sync")
    FString GetSharedLedgerFilePath() const;
};
```

```cpp
// WonderLedgerSubsystem.cpp - Unreal Engine 5.8
#include "WonderLedgerSubsystem.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void UWonderLedgerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    AuditAndBalanceLedger();
}

void UWonderLedgerSubsystem::RecordGrindIncome(float CashEarned, int32 VitalityCreditsEarned, const FString& SourceLabel)
{
    Column1Grind.CNAAndWetVacMiniGames += FMath::Max(0.0f, CashEarned);
    Column3Foundation.VitalityCreditsTier1 += FMath::Max(0, VitalityCreditsEarned);

    AuditAndBalanceLedger();
    SaveSharedLedgerJson();

    FString MamaMsg = FString::Printf(TEXT("Baby, that %s hustle brought in $%.0f and %d Vitality Credits! Just make sure you rest your legs."), *SourceLabel, CashEarned, VitalityCreditsEarned);
    FString PapaMsg = FString::Printf(TEXT("That's what I'm talking about! Column 1 is climbing and we're movin' on up to Era %d!"), static_cast<int32>(CurrentEra) + 1);
    OnMamaPapaVoiceAdvisory.Broadcast(MamaMsg, PapaMsg);
}

void UWonderLedgerSubsystem::ApplyOverheadRelief(float UtilityReductionAmount)
{
    Column2Overhead.UtilityGridLoadCost = FMath::Max(500.0f, Column2Overhead.UtilityGridLoadCost - UtilityReductionAmount);
    AuditAndBalanceLedger();
    SaveSharedLedgerJson();
}

bool UWonderLedgerSubsystem::AuditAndBalanceLedger()
{
    const float Gross = Column1Grind.GetTotalGrossRevenue();
    const float Overhead = Column2Overhead.GetTotalOperatingExpenses();
    const float NetSurplus = Gross - Overhead;

    Column3Foundation.LiquidReserve = NetSurplus;
    bEvonHubPermissionGranted = (NetSurplus >= 0.0f);
    Column3Foundation.bEmissiveSignageActive = bEvonHubPermissionGranted;

    // Check Era progression based on Industrial Bonds
    if (Column3Foundation.IndustrialBondsTier2And3 >= 500)
        CurrentEra = EWonderEra::Era5_PurpleCity;
    else if (Column3Foundation.IndustrialBondsTier2And3 >= 300)
        CurrentEra = EWonderEra::Era4_LunarAnchor;
    else if (Column3Foundation.IndustrialBondsTier2And3 >= 180)
        CurrentEra = EWonderEra::Era3_OrbitalRing;
    else if (Column3Foundation.IndustrialBondsTier2And3 >= 100)
        CurrentEra = EWonderEra::Era2_CloudGrid;
    else
        CurrentEra = EWonderEra::Era1_ConcreteGrind;

    OnLedgerUpdated.Broadcast(Gross, Overhead, Column3Foundation.LiquidReserve);
    return bEvonHubPermissionGranted;
}

bool UWonderLedgerSubsystem::ConvertSurplusToIndustrialBonds(float ReserveAmountToConvert)
{
    if (!bEvonHubPermissionGranted || Column3Foundation.LiquidReserve < ReserveAmountToConvert || ReserveAmountToConvert < 1000.0f)
    {
        OnMamaPapaVoiceAdvisory.Broadcast(
            TEXT("Hold on, sugar! We have to cover Column 2 Overhead bills before we issue more tower bonds."),
            TEXT("Mama's right! Balance Column 2 first so the Evon Hub gives us PERMISSION GRANTED!")
        );
        return false;
    }

    const int32 BondsMinted = FMath::FloorToInt(ReserveAmountToConvert / 1000.0f);
    Column3Foundation.LiquidReserve -= (BondsMinted * 1000.0f);
    Column3Foundation.IndustrialBondsTier2And3 += BondsMinted;
    Column3Foundation.UnlockedTowerTier = FMath::Clamp(1 + (Column3Foundation.IndustrialBondsTier2And3 / 35), 1, 10);

    AuditAndBalanceLedger();
    SaveSharedLedgerJson();
    return true;
}

FString UWonderLedgerSubsystem::GetSharedLedgerFilePath() const
{
    return FPaths::Combine(FPaths::ProjectDir(), TEXT(".."), TEXT("02_Shared_Game_Core"), TEXT("State"), TEXT("WonderLedgerState.json"));
}

bool UWonderLedgerSubsystem::SaveSharedLedgerJson()
{
    FString JsonStr = FString::Printf(
        TEXT("{\n")
        TEXT("  \"schema_version\": \"3.0.0\",\n")
        TEXT("  \"tower_name\": \"Mama Wonder Tower\",\n")
        TEXT("  \"current_era\": %d,\n")
        TEXT("  \"hardware_profile\": \"RTX_5060_32GB_RAM\",\n")
        TEXT("  \"sound_magic_bpm\": 85,\n")
        TEXT("  \"evon_hub_permission_granted\": %s,\n")
        TEXT("  \"ledger_columns\": {\n")
        TEXT("    \"column_1_gross_revenue\": {\n")
        TEXT("      \"cna_and_wetvac_minigames\": %.2f,\n")
        TEXT("      \"arcade_brawler_payouts\": %.2f,\n")
        TEXT("      \"commercial_lease_income\": %.2f,\n")
        TEXT("      \"cities_skylines_tourism_bonus\": %.2f\n")
        TEXT("    },\n")
        TEXT("    \"column_2_operating_expenses\": {\n")
        TEXT("      \"structural_maintenance\": %.2f,\n")
        TEXT("      \"security_and_brawler_staff\": %.2f,\n")
        TEXT("      \"utility_grid_load_cost\": %.2f\n")
        TEXT("    },\n")
        TEXT("    \"column_3_net_reserve_and_upgrades\": {\n")
        TEXT("      \"liquid_reserve\": %.2f,\n")
        TEXT("      \"vitality_credits_tier1\": %d,\n")
        TEXT("      \"industrial_bonds_tier2_3\": %d,\n")
        TEXT("      \"unlocked_tier\": %d,\n")
        TEXT("      \"cities_skylines_lot_size\": \"%s\",\n")
        TEXT("      \"emissive_signage_active\": %s\n")
        TEXT("    }\n")
        TEXT("  }\n")
        TEXT("}\n"),
        static_cast<int32>(CurrentEra) + 1,
        bEvonHubPermissionGranted ? TEXT("true") : TEXT("false"),
        Column1Grind.CNAAndWetVacMiniGames,
        Column1Grind.ArcadeBrawlerPayouts,
        Column1Grind.CommercialLeaseIncome,
        Column1Grind.CitiesSkylinesTourismBonus,
        Column2Overhead.StructuralMaintenance,
        Column2Overhead.SecurityAndBrawlerStaff,
        Column2Overhead.UtilityGridLoadCost,
        Column3Foundation.LiquidReserve,
        Column3Foundation.VitalityCreditsTier1,
        Column3Foundation.IndustrialBondsTier2And3,
        Column3Foundation.UnlockedTowerTier,
        *Column3Foundation.CitiesSkylinesLotSize,
        Column3Foundation.bEmissiveSignageActive ? TEXT("true") : TEXT("false")
    );

    return FFileHelper::SaveStringToFile(JsonStr, *GetSharedLedgerFilePath());
}
```

---

### 9.2. Unreal Engine 5.8 C++ — Wet-Vac 59,000-Gallon Extraction & CNA Shift Grind Mini-Games (`WonderGrindMiniGameComponent.h` & `.cpp`)
Attach this ActorComponent to `BP_DarrellXGen` or your Street District Manager in `03_UE58_WonderWorld\Source\WonderWorld\`:

```cpp
// WonderGrindMiniGameComponent.h - Unreal Engine 5.8
// Mama's Wonder World: Era 1 "Concrete Grind" Mini-Games:
// 1) 59,000-Gallon Wet-Vac Flood Extraction Challenge
// 2) CNA CareShift & Stamina Pacing Mini-Game (Hoyer Lift, Vital Signs, Mama Rest Elevation)
// 3) 85 BPM Electrical Grid Load Balancer

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WonderGrindMiniGameComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWetVacProgress, float, GallonsRemaining, float, CurrentTankGallons, bool, bFoundationCleared);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCNAShiftProgress, int32, PatientsCaredFor, float, CurrentStamina, float, ShiftComboMultiplier);

UCLASS(ClassGroup=(WonderWorld), meta=(BlueprintSpawnableComponent))
class WONDERWORLD_API UWonderGrindMiniGameComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWonderGrindMiniGameComponent();

    // =========================================================================
    // MINI-GAME 1: 59,000-GALLON WET-VAC FLOOD EXTRACTION
    // =========================================================================
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grind Mini-Games|Wet-Vac")
    float TotalFloodGallonsToClear = 59000.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grind Mini-Games|Wet-Vac")
    float RemainingFloodGallons = 59000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grind Mini-Games|Wet-Vac")
    float WetVacTankCapacity = 2500.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grind Mini-Games|Wet-Vac")
    float CurrentWetVacTankGallons = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grind Mini-Games|Wet-Vac")
    bool bSubterraneanFoundationDry = false;

    UPROPERTY(BlueprintAssignable, Category = "Grind Mini-Games|Wet-Vac")
    FOnWetVacProgress OnWetVacProgress;

    // Pump the Wet-Vac! Hitting on the 85 BPM Sound Magic beat gives 1.5x suction power
    UFUNCTION(BlueprintCallable, Category = "Grind Mini-Games|Wet-Vac")
    void PumpWetVacSuction(bool bSyncedTo85BPMBeat);

    // Empty the Wet-Vac tank into the storm drain to cash out Column 1 Vitality Credits
    UFUNCTION(BlueprintCallable, Category = "Grind Mini-Games|Wet-Vac")
    void DumpWetVacTankAndCashOut();

    // =========================================================================
    // MINI-GAME 2: CNA SHIFT, HOYER LIFT & MAMA STAMINA PACING
    // =========================================================================
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grind Mini-Games|CNA Shift")
    float PlayerStamina = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grind Mini-Games|CNA Shift")
    float LegEdemaPressure = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grind Mini-Games|CNA Shift")
    int32 CompletedPatientTasks = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grind Mini-Games|CNA Shift")
    float CareQualityMultiplier = 1.0f;

    UPROPERTY(BlueprintAssignable, Category = "Grind Mini-Games|CNA Shift")
    FOnCNAShiftProgress OnCNAShiftProgress;

    // Perform CNA care task: "HoyerLiftTransfer", "VitalSignsCheck", or "ADLPatientCare"
    UFUNCTION(BlueprintCallable, Category = "Grind Mini-Games|CNA Shift")
    bool ExecuteCNACareTask(const FString& TaskType);

    // Mama AI's Couch Leg-Elevation Rest Break — lowers edema pressure & restores stamina
    UFUNCTION(BlueprintCallable, Category = "Grind Mini-Games|CNA Shift")
    void TakeMamaElevationRestBreak();

    // Complete the CNA shift and send payouts to UWonderLedgerSubsystem
    UFUNCTION(BlueprintCallable, Category = "Grind Mini-Games|CNA Shift")
    void CompleteCNAShiftAndDeposit();

    // =========================================================================
    // MINI-GAME 3: 85 BPM ELECTRICAL GRID BALANCER
    // =========================================================================
    UFUNCTION(BlueprintCallable, Category = "Grind Mini-Games|Electrical Grid")
    bool BalanceElectricalPhaseOnBeat(float PhaseTimingAccuracy);
};
```

```cpp
// WonderGrindMiniGameComponent.cpp - Unreal Engine 5.8
#include "WonderGrindMiniGameComponent.h"
#include "WonderLedgerSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

UWonderGrindMiniGameComponent::UWonderGrindMiniGameComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UWonderGrindMiniGameComponent::PumpWetVacSuction(bool bSyncedTo85BPMBeat)
{
    if (bSubterraneanFoundationDry || CurrentWetVacTankGallons >= WetVacTankCapacity)
    {
        return;
    }

    const float BaseSuctionGallons = bSyncedTo85BPMBeat ? 750.0f : 450.0f;
    const float SpaceInTank = WetVacTankCapacity - CurrentWetVacTankGallons;
    const float Extracted = FMath::Min3(BaseSuctionGallons, SpaceInTank, RemainingFloodGallons);

    CurrentWetVacTankGallons += Extracted;
    RemainingFloodGallons -= Extracted;

    if (RemainingFloodGallons <= 0.0f)
    {
        RemainingFloodGallons = 0.0f;
        bSubterraneanFoundationDry = true;
    }

    OnWetVacProgress.Broadcast(RemainingFloodGallons, CurrentWetVacTankGallons, bSubterraneanFoundationDry);
}

void UWonderGrindMiniGameComponent::DumpWetVacTankAndCashOut()
{
    if (CurrentWetVacTankGallons <= 0.0f) return;

    const float CashReward = (CurrentWetVacTankGallons / 100.0f) * 45.0f;
    const int32 VitalityReward = FMath::RoundToInt((CurrentWetVacTankGallons / 100.0f) * 18.0f);
    CurrentWetVacTankGallons = 0.0f;

    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GI = World->GetGameInstance())
        {
            if (UWonderLedgerSubsystem* Ledger = GI->GetSubsystem<UWonderLedgerSubsystem>())
            {
                Ledger->RecordGrindIncome(CashReward, VitalityReward, TEXT("59,000-Gallon Wet-Vac Extraction"));
                if (bSubterraneanFoundationDry)
                {
                    // Clearing the 59,000-gallon leak also cuts Column 2 utility/maintenance overhead!
                    Ledger->ApplyOverheadRelief(653.91f);
                }
            }
        }
    }

    OnWetVacProgress.Broadcast(RemainingFloodGallons, CurrentWetVacTankGallons, bSubterraneanFoundationDry);
}

bool UWonderGrindMiniGameComponent::ExecuteCNACareTask(const FString& TaskType)
{
    float StaminaCost = 14.0f;
    float EdemaIncrease = 12.0f;

    if (TaskType.Equals(TEXT("HoyerLiftTransfer"), ESearchCase::IgnoreCase))
    {
        StaminaCost = 22.0f;
        EdemaIncrease = 18.0f;
    }
    else if (TaskType.Equals(TEXT("VitalSignsCheck"), ESearchCase::IgnoreCase))
    {
        StaminaCost = 10.0f;
        EdemaIncrease = 8.0f;
    }

    // Mama AI safety rule: never let the player over-exert when stamina is low or leg edema is high
    if (PlayerStamina < StaminaCost || LegEdemaPressure >= 85.0f)
    {
        return false; // Prompt player to call TakeMamaElevationRestBreak()
    }

    PlayerStamina = FMath::Clamp(PlayerStamina - StaminaCost, 0.0f, 100.0f);
    LegEdemaPressure = FMath::Clamp(LegEdemaPressure + EdemaIncrease, 0.0f, 100.0f);
    CompletedPatientTasks++;
    CareQualityMultiplier = FMath::Clamp(CareQualityMultiplier + 0.15f, 1.0f, 3.0f);

    OnCNAShiftProgress.Broadcast(CompletedPatientTasks, PlayerStamina, CareQualityMultiplier);
    return true;
}

void UWonderGrindMiniGameComponent::TakeMamaElevationRestBreak()
{
    // Elevating legs on the sofa pillows relieves bilateral leg swelling and restores breathing stamina
    PlayerStamina = FMath::Clamp(PlayerStamina + 45.0f, 0.0f, 100.0f);
    LegEdemaPressure = FMath::Clamp(LegEdemaPressure - 50.0f, 0.0f, 100.0f);
    OnCNAShiftProgress.Broadcast(CompletedPatientTasks, PlayerStamina, CareQualityMultiplier);
}

void UWonderGrindMiniGameComponent::CompleteCNAShiftAndDeposit()
{
    if (CompletedPatientTasks <= 0) return;

    const float ShiftCash = CompletedPatientTasks * 320.0f * CareQualityMultiplier;
    const int32 ShiftVitalityCredits = FMath::RoundToInt(CompletedPatientTasks * 110.0f * CareQualityMultiplier);

    CompletedPatientTasks = 0;
    CareQualityMultiplier = 1.0f;

    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GI = World->GetGameInstance())
        {
            if (UWonderLedgerSubsystem* Ledger = GI->GetSubsystem<UWonderLedgerSubsystem>())
            {
                Ledger->RecordGrindIncome(ShiftCash, ShiftVitalityCredits, TEXT("CNA CareShift & Patient Advocacy"));
            }
        }
    }
}

bool UWonderGrindMiniGameComponent::BalanceElectricalPhaseOnBeat(float PhaseTimingAccuracy)
{
    if (PhaseTimingAccuracy >= 0.80f)
    {
        if (UWorld* World = GetWorld())
        {
            if (UGameInstance* GI = World->GetGameInstance())
            {
                if (UWonderLedgerSubsystem* Ledger = GI->GetSubsystem<UWonderLedgerSubsystem>())
                {
                    Ledger->ApplyOverheadRelief(450.0f);
                    Ledger->RecordGrindIncome(600.0f, 250, TEXT("85 BPM Electrical Grid Overcharge"));
                }
            }
        }
        return true;
    }
    return false;
}
```

---

### 9.3. Unity 2022.3 LTS C# — Unified 3-Column Ledger & Wet-Vac / CNA Mini-Game Controller (`WonderLedgerAndGrindController.cs`)
Save in `04_Unity_Brawler_CS2\Assets\WonderShared\WonderLedgerAndGrindController.cs`:

```csharp
using System;
using System.IO;
using UnityEngine;
using UnityEngine.Events;

namespace WonderWorld.EconomyAndMinigames
{
    public class WonderLedgerAndGrindController : MonoBehaviour
    {
        [Header("Column 1: The Grind (Gross Revenue)")]
        public float cnaAndWetVacMiniGames = 8500f;
        public float arcadeBrawlerPayouts = 14500f;
        public float commercialLeaseIncome = 48000f;
        public float citiesSkylinesTourismBonus = 9250f;

        [Header("Column 2: The Overhead (Survival Gate)")]
        public float structuralMaintenance = 11200f;
        public float securityAndBrawlerStaff = 8400f;
        public float utilityGridLoadCost = 5100f;

        [Header("Column 3: The Foundation (Wonder Capital)")]
        public float liquidReserve = 55550f;
        public int vitalityCreditsTier1 = 12400;
        public int industrialBondsTier2And3 = 85;
        public int unlockedTier = 3;
        public bool evonHubPermissionGranted = true;

        [Header("Era 1 Mini-Game: 59,000-Gallon Wet-Vac Extraction")]
        public float remainingFloodGallons = 59000f;
        public float currentWetVacTank = 0f;
        public float maxWetVacTank = 2500f;

        [Header("Era 1 Mini-Game: CNA Shift & Mama Stamina Pacing")]
        public float playerStamina = 100f;
        public float legEdemaPressure = 0f;
        public int completedCNATasks = 0;

        public UnityEvent<string, string> onMamaPapaDialogue;

        public float GetColumn1Gross() =>
            cnaAndWetVacMiniGames + arcadeBrawlerPayouts + commercialLeaseIncome + citiesSkylinesTourismBonus;

        public float GetColumn2Overhead() =>
            structuralMaintenance + securityAndBrawlerStaff + utilityGridLoadCost;

        public void PumpWetVac(bool hit85BpmBeat)
        {
            if (remainingFloodGallons <= 0f || currentWetVacTank >= maxWetVacTank) return;
            float pumped = Mathf.Min(hit85BpmBeat ? 750f : 450f, maxWetVacTank - currentWetVacTank, remainingFloodGallons);
            currentWetVacTank += pumped;
            remainingFloodGallons -= pumped;

            if (currentWetVacTank >= maxWetVacTank || remainingFloodGallons <= 0f)
            {
                DumpWetVacTank();
            }
        }

        public void DumpWetVacTank()
        {
            if (currentWetVacTank <= 0f) return;
            float earnedCash = (currentWetVacTank / 100f) * 45f;
            int earnedVitality = Mathf.RoundToInt((currentWetVacTank / 100f) * 18f);
            currentWetVacTank = 0f;

            cnaAndWetVacMiniGames += earnedCash;
            vitalityCreditsTier1 += earnedVitality;

            if (remainingFloodGallons <= 0f)
            {
                utilityGridLoadCost = Mathf.Max(500f, utilityGridLoadCost - 653.91f);
            }

            RecalculateLedger();
            onMamaPapaDialogue?.Invoke(
                $"Baby, you pumped that flood down! Earned ${earnedCash:F0} and {earnedVitality} Vitality Credits!",
                "Minime Tophatter just locked the foundation light-codes! Column 3 is ready to build!"
            );
        }

        public void PerformCNATask(bool isHeavyHoyerLift)
        {
            float cost = isHeavyHoyerLift ? 20f : 12f;
            if (playerStamina < cost || legEdemaPressure >= 85f)
            {
                onMamaPapaDialogue?.Invoke(
                    "Sugar, sit down and elevate your legs on the sofa pillows first! Protect your breathing and stamina.",
                    "Listen to Big Mama! Take a quick rest break so you keep your shift multiplier!"
                );
                return;
            }

            playerStamina -= cost;
            legEdemaPressure += isHeavyHoyerLift ? 16f : 9f;
            completedCNATasks++;
            cnaAndWetVacMiniGames += isHeavyHoyerLift ? 450f : 275f;
            vitalityCreditsTier1 += isHeavyHoyerLift ? 150 : 95;
            RecalculateLedger();
        }

        public void TakeMamaRestBreak()
        {
            playerStamina = Mathf.Clamp(playerStamina + 45f, 0f, 100f);
            legEdemaPressure = Mathf.Clamp(legEdemaPressure - 50f, 0f, 100f);
        }

        public void RecalculateLedger()
        {
            liquidReserve = GetColumn1Gross() - GetColumn2Overhead();
            evonHubPermissionGranted = liquidReserve >= 0f;
        }
    }
}
```
