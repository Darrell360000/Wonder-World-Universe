@echo off
title MAMA'S WONDER WORLD // HOUSE PARTY CLI & ANTIGRAVITY HUB
color 0D
cd /d "%~dp0"
set "BLENDER_EXE=C:\Program Files\Blender Foundation\Blender 5.2\blender.exe"

:MENU
cls
echo ============================================================================
echo   MAMA'S WONDER WORLD: PURPLE CITY UNIVERSE // HOUSE PARTY CLI HUB
echo   Protagonist: Darrell X-Gen (Black Velvet Top Hat + Rainbow LED EQ Band)
echo   Empire: Mama Tower ^& Papa Tower (150-Story Twin Towers + Skybridge)
echo ============================================================================
echo.
echo   [1] PLAY 3D GAME NOW (Launch Street Hat 3D Launcher in Browser)
echo   [2] PLAY OPEN-WORLD SANDBOX (3rd-Person Walk, Street Cars, Flying Cars ^& City Builder)
echo   [3] BUILD DARRELL X-GEN 3D ASSET FOR UE5 (Blender Headless Pipeline)
echo   [4] BUILD FULL METROPOLIS ^& TWIN TOWERS 3D ASSETS (Blender Pipeline)
echo   [5] VALIDATE UNREAL ENGINE 5 C++ ^& UNITY 6 C# CONTRACTS
echo   [6] SYNC ^& PUSH EVERYTHING TO GITHUB (Antigravity + Copilot Sync)
echo   [7] OPEN PROJECT IN VS CODE (Antigravity / GitHub Copilot Ready)
echo   [8] LAUNCH UNREAL ENGINE 5.8 EDITOR + AI COPILOT CLI
echo   [9] LAUNCH UNIFIED DEVELOPER POWERSHELL & BUILD PIPELINE BRIDGE
echo   [10] LAUNCH UNIFIED WORKSPACE BRIDGE (Master 35-App Launcher)
echo   [0] EXIT
echo.
set /p choice="Select an option [0-10]: "

if "%choice%"=="1" (
    echo Launching Mama's Wonder World 3D WebGL Game...
    start "" "%~dp0index.html"
    pause
    goto MENU
)
if "%choice%"=="2" (
    echo Launching Mama's Wonder World Open-World 3D Sandbox...
    start "" "%~dp0WonderWorld_OpenWorld_Sandbox.html"
    pause
    goto MENU
)
if "%choice%"=="3" (
    echo Building Darrell X-Gen Exact Likeness 3D Mesh + UE5 Skeleton...
    "%BLENDER_EXE%" --background --python build_darrell_xgen_to_ue5.py -- --output Assets/WonderWorld/Characters/DarrellXGen
    pause
    goto MENU
)
if "%choice%"=="4" (
    echo Building Wonder World Hats, Twin Towers ^& Metropolis District...
    "%BLENDER_EXE%" --background --python Scripts/wonder_world_blender_cli.py -- --output Assets/WonderWorld/Generated
    "%BLENDER_EXE%" --background --python Scripts/generate_metropolis_district.py -- --output Assets/WonderWorld/Environment
    pause
    goto MENU
)
if "%choice%"=="5" (
    echo Running Unreal Engine 5 C++ ^& Unity 6 C# Validation...
    py Scripts/validate_municipal_subsystems.py
    dotnet build WonderWorldUnity/WonderWorldValidation.csproj --configuration Release
    pause
    goto MENU
)
if "%choice%"=="6" (
    echo Syncing with GitHub...
    powershell -ExecutionPolicy Bypass -File "%~dp0Sync-WonderWorldToGitHub.ps1"
    pause
    goto MENU
)
if "%choice%"=="7" (
    echo Opening workspace in VS Code...
    code "%~dp0"
    goto MENU
)
if "%choice%"=="8" (
    echo Launching Unreal Engine 5.8 Editor + AI Copilot CLI...
    call "C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon\LAUNCH_UNREAL_AND_COPILOT.bat"
    goto MENU
)
if "%choice%"=="9" (
    echo Launching Unified Developer PowerShell Build Pipeline Bridge...
    call "%~dp0LAUNCH_UNIFIED_DEVELOPER_PIPELINE.bat"
    goto MENU
)
if "%choice%"=="10" (
    echo Launching Unified Workspace Bridge (Invoke-AntigravityMasterBridge.ps1)...
    powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Scripts\Invoke-AntigravityMasterBridge.ps1"
    goto MENU
)
if "%choice%"=="0" exit
goto MENU
