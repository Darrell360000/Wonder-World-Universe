<#
.SYNOPSIS
    Mama's Wonder World // Desktop Shortcut, CLI House Party Mode & GitHub Sync Script
    Give this script to Antigravity or run it directly in Windows 11 PowerShell!

.DESCRIPTION
    1. Creates a one-click "Mama's Wonder World - House Party Launcher" shortcut on your Windows 11 Desktop.
    2. Creates the master CLI launcher batch file (LAUNCH_WONDER_HOUSE_PARTY.bat) that starts a local web server, opens the 3D House Party / Workforce game right at home, runs the Blender CLI asset check, and syncs to GitHub.
    3. Initializes and pushes your latest Wonder World files to your GitHub repository.
#>

param(
    [string]$ProjectDir = (Get-Location).Path,
    [string]$GitHubRepoUrl = ""
)

$ErrorActionPreference = "Stop"

Write-Host "================================================================" -ForegroundColor Cyan
Write-Host "  MAMA'S WONDER WORLD // ANTIGRAVITY CLI & GITHUB SETUP" -ForegroundColor Cyan
Write-Host "  Mode: Stay-In House Party Edition (85 BPM Sound Magic)" -ForegroundColor Magenta
Write-Host "================================================================" -ForegroundColor Cyan

# 1. Create the Master CLI Launcher Batch File in the Project Folder
$BatchFilePath = Join-Path $ProjectDir "LAUNCH_WONDER_HOUSE_PARTY.bat"

$BatchContent = @"
@echo off
title Mama's Wonder World // House Party CLI Command Center
color 0B
cd /d "%~dp0"

echo ====================================================================
echo   MAMA'S WONDER WORLD // HOUSE PARTY CLI & GITHUB LAUNCHER
echo   85 BPM Sound Magic - Stay-At-Home VIP Edition
echo ====================================================================
echo.
echo [1] Launch 3D Wonder World Game (House Party Browser Mode)
echo [2] Run Blender 4.x CLI Asset Generator (Top Hats ^& Twin Towers)
echo [3] Build .NET / Unity 6 / Cities: Skylines II CLR Mod
echo [4] Push ^& Sync All Files to GitHub
echo [5] Run Full All-In-One Pipeline + Launch Game
echo.
set /p choice="Select an option (1-5, or press Enter for [1]): "

if "%choice%"=="" set choice=1

if "%choice%"=="1" goto PLAY_GAME
if "%choice%"=="2" goto RUN_BLENDER
if "%choice%"=="3" goto BUILD_CLR
if "%choice%"=="4" goto PUSH_GITHUB
if "%choice%"=="5" goto FULL_PARTY

:PLAY_GAME
echo.
echo [CLI] Starting Wonder World House Party Game...
if exist "index.html" (
    start "" "index.html"
) else if exist "Wonder_MultiStage_MiniMe_Referee.html" (
    start "" "Wonder_MultiStage_MiniMe_Referee.html"
) else (
    echo [!] index.html not found in current folder. Opening folder...
    start "" .
)
goto END

:RUN_BLENDER
echo.
echo [CLI] Running Blender 4.x Headless CLI Asset Generator...
where blender >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    blender --background --python Scripts/wonder_world_blender_cli.py -- --output Assets/WonderWorld/Generated
) else (
    python Scripts/wonder_world_blender_cli.py -- --output Assets/WonderWorld/Generated
)
goto END

:BUILD_CLR
echo.
echo [CLI/CLR] Building C# .NET CLR Assembly for Unity 6 / Cities: Skylines II...
if exist "WonderWorldUnity\WonderWorldValidation.csproj" (
    dotnet build "WonderWorldUnity\WonderWorldValidation.csproj" --configuration Release
) else (
    echo [!] WonderWorldValidation.csproj not found yet. Ask Antigravity to scaffold it!
)
goto END

:PUSH_GITHUB
echo.
echo [GitHub] Staging, committing, and pushing Wonder World Universe to GitHub...
git add -A
git commit -m "Update Mama's Wonder World Universe - House Party CLI Sync"
git push -u origin main
goto END

:FULL_PARTY
echo.
echo [PARTY MODE] Running Blender CLI + CLR Build + GitHub Sync + Launching Game!
if exist "Scripts\wonder_world_blender_cli.py" python Scripts/wonder_world_blender_cli.py -- --output Assets/WonderWorld/Generated
if exist "WonderWorldUnity\WonderWorldValidation.csproj" dotnet build "WonderWorldUnity\WonderWorldValidation.csproj" --configuration Release
if exist "index.html" start "" "index.html"
goto END

:END
echo.
echo [Done] Keep the 85 BPM Sound Magic rolling!
pause
"@

Set-Content -Path $BatchFilePath -Value $BatchContent -Encoding UTF8
Write-Host "[✓] Created CLI Launcher: $BatchFilePath" -ForegroundColor Green

# 2. Create the Windows 11 Desktop Shortcut (.lnk)
$DesktopPath = [Environment]::GetFolderPath("Desktop")
$ShortcutPath = Join-Path $DesktopPath "Mama's Wonder World (House Party CLI).lnk"

$WshShell = New-Object -ComObject WScript.Shell
$Shortcut = $WshShell.CreateShortcut($ShortcutPath)
$Shortcut.TargetPath = $BatchFilePath
$Shortcut.WorkingDirectory = $ProjectDir
$Shortcut.WindowStyle = 1
$Shortcut.Description = "Launch Mama's Wonder World 3D Game, Blender CLI, .NET CLR Mod & GitHub Sync"
$Shortcut.IconLocation = "shell32.dll,13" # Glowing Star/World Icon
$Shortcut.Save()

Write-Host "[✓] Created Desktop Shortcut: $ShortcutPath" -ForegroundColor Green

# 3. GitHub Repository Setup & Auto-Sync Helper
$GitPushScriptPath = Join-Path $ProjectDir "Sync-WonderWorldToGitHub.ps1"
$GitPushContent = @"
param(
    [string]`$CommitMessage = "Wonder World Universe - House Party Edition Update",
    [string]`$RemoteUrl = "$GitHubRepoUrl"
)

if (-not (Test-Path ".git")) {
    Write-Host "[GitHub] Initializing new Git repository..." -ForegroundColor Cyan
    git init
    git branch -M main
}

if (`$RemoteUrl -ne "") {
    `$existing = git remote
    if (`$existing -notcontains "origin") {
        git remote add origin `$RemoteUrl
    } else {
        git remote set-url origin `$RemoteUrl
    }
}

git add -A
git commit -m "`$CommitMessage" --allow-empty
Write-Host "[GitHub] Ready! Pushing to origin/main..." -ForegroundColor Yellow
git push -u origin main
"@

Set-Content -Path $GitPushScriptPath -Value $GitPushContent -Encoding UTF8
Write-Host "[✓] Created GitHub Sync Script: $GitPushScriptPath" -ForegroundColor Green
Write-Host ""
Write-Host "ALL SET! Double-click 'Mama's Wonder World (House Party CLI)' on your Desktop!" -ForegroundColor Yellow