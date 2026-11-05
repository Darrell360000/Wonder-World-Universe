@echo off
title MAMA'S WONDER WORLD // UNIFIED DEVELOPER CMD BRIDGE
color 0B
cd /d "%~dp0"

echo ============================================================================
echo   MAMA'S WONDER WORLD: UNIFIED DEVELOPER COMMAND PROMPT BRIDGE
echo   Configuring Visual Studio 2022/2026 x64 Build Environment...
echo ============================================================================

set "VS_DEV_CMD=C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
if not exist "%VS_DEV_CMD%" (
    set "VS_DEV_CMD=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat"
)

if exist "%VS_DEV_CMD%" (
    echo Sourcing: %VS_DEV_CMD%
    call "%VS_DEV_CMD%" -arch=x64
) else (
    echo [WARNING] Visual Studio VsDevCmd.bat not found. Proceeding with standard environment.
)

REM Inject Engine Toolchain paths
set "PATH=C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64;C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\DotNET\UnrealBuildTool;C:\Program Files\Blender Foundation\Blender 5.2;C:\Program Files\dotnet;C:\Program Files\Git\cmd;%PATH%"

echo.
echo Engine Toolchain and Visual Studio x64 Build Environment Initialized.
echo Project Directory: %CD%
echo.
cmd.exe /k "prompt [WonderWorld-CMD] $P$G"
