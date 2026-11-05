@echo off
title MAMA'S WONDER WORLD // UNIFIED DEVELOPER POWERSHELL & BUILD PIPELINE BRIDGE
color 0D
cd /d "%~dp0"

echo ============================================================================
echo   MAMA'S WONDER WORLD: PURPLE CITY UNIVERSE // DEVELOPER PIPELINE BRIDGE
echo   Bridging: MSVC 2022/2026, UE 5.8, Blender 5.2, Unity 6/CS2 ECS, Git, S23
echo ============================================================================
echo.
echo Launching Native 64-Bit Developer PowerShell Environment...
echo.

set "POWERSHELL_EXE=%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe"
if not exist "%POWERSHELL_EXE%" (
    set "POWERSHELL_EXE=powershell.exe"
)

"%POWERSHELL_EXE%" -NoExit -ExecutionPolicy Bypass -File "%~dp0Scripts\Unified-DeveloperPowerShell-Bridge.ps1"
