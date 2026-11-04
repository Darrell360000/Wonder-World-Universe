# Wonder World: Antigravity + GitHub Copilot

This project uses Antigravity or another local development agent for Windows CLI work, GitHub Copilot for in-editor assistance, and GitHub Actions for hosted validation and asset artifacts.

## Darrell X-Gen Blender asset

The canonical design target is described in `.github/copilot-instructions.md`. The reference photo mentioned by the original brief was not included, so the generated character is an original stylized interpretation rather than a verified likeness.

Generate a Blender scene and interchange exports with:

```powershell
blender --background --python Scripts/build_darrell_xgen_to_ue5.py -- --output Assets/WonderWorld/Characters/DarrellXGen
```

The asset design includes a matte-black top hat with a five-color LED equalizer band, a pinstripe suit, sculpted facial hair, and a cyan-and-gold forearm gauntlet. Rhythm-driven lighting, parries, and combat behavior require separate runtime integration.

## Validation

```powershell
python Scripts/validate_wonder_ledger.py
python Scripts/validate_municipal_subsystems.py
python Scripts/validate_unity_source_contract.py
dotnet build WonderWorldUnity/WonderWorldValidation.csproj --configuration Release
```

The Unity validation project depends on a local Unity installation and may require adjusting its configured editor assembly paths. The hosted pipeline validates Unreal and Unity/Cities: Skylines II source contracts; compiling those projects requires their respective editor toolchains and plugins.

The GitHub Actions workflow validates the shared ledger and source contracts, generates Blender assets, and uploads asset and game-core artifacts. It does not publish a live GitHub Pages game. Commit, push, or publish only when explicitly requested.
