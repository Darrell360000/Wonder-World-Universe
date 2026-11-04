@echo off
setlocal
cd /d "%~dp0"
powershell.exe -ExecutionPolicy Bypass -File "%~dp0Scripts\Launch-Unreal-Blender-AICopilot.ps1"
