# WonderWorld Unity Play Slice

This standalone project contains the requested Unity-side dual-camera, brawler combat, and escort mission controllers. It is kept separately from the Unreal project.

## Scene wiring

1. Open `WonderWorldUnity` in Unity 2022.3.62f2.
2. Create a scene named `WonderWorldPlaySlice`.
3. Create `StrategyCamera` and `BrawlerCamera`, add a `WonderCameraDirector` to an empty object, and assign both cameras.
4. Add `BrawlerCombatController` to the player. Route Input/animation/UI to `TryAttack`, `ReceiveDamage`, and its UnityEvents.
5. Add `EscortMissionController` to a mission manager. Assign a destination and every escort subject. Call `StartMission`; call `ReportEscortDamage(subject, amount)` from damage sources.

`TryAttack` uses a sphere overlap in front of the player and only damages components implementing `WonderWorld.IDamageable`. Drive feedback through `onAttackConnected` (combo step and damage), and health through `onHealthChanged`.

## Validation

- Runtime C# assembly: `dotnet build WonderWorldUnity/WonderWorldValidation.csproj --configuration Release`. The project defaults to the Unity 2022.3.62f2 Hub install path; override with `-p:UnityEditorPath="C:\Program Files\Unity\Hub\Editor\<version>"` when needed.
- In Unity Test Runner, run the full `EditMode` suite under `Assets/Scripts/Tests` (brawler hit deduplication/damage safety, escort progress/failure, and interrupted camera transitions).
