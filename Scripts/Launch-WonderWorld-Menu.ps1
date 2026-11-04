# Mama's Wonder World // Master Desktop Control Center
$ErrorActionPreference = "Continue"
$UnrealProject = "C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon"
$UniverseRepo  = "C:\Users\Admin\.gemini\antigravity\scratch\Wonder-World-Universe"
$OpenWorldHtml = "$UniverseRepo\WonderWorld_OpenWorld_Sandbox.html"
$GameHtml      = "$UniverseRepo\index.html"
$CS2Exe        = "D:\Games\Cities - Skylines II\Cities2.exe"

while ($true) {
    Clear-Host
    Write-Host "==============================================================================" -ForegroundColor Magenta
    Write-Host "   🎩 MAMA'S WONDER WORLD // UNREAL 5.8 + BLENDER CLI + OPEN-WORLD HUB" -ForegroundColor Cyan
    Write-Host "==============================================================================" -ForegroundColor Magenta
    Write-Host ""
    Write-Host "  [1] 🛸 Play Open-World 3D Sandbox (Third-Person, Street Driving, Flying Cars & City Builder)" -ForegroundColor Green
    Write-Host "  [2] 🚀 Launch Unreal Engine 5.8 + Blender 5.2.3 CLI + AI Copilot (WonderTowerTycoon)" -ForegroundColor Cyan
    Write-Host "  [3] 🎮 Play Wonder Hat Street Launcher & Twin Tower Tycoon (WebGL)" -ForegroundColor Green
    Write-Host "  [4] 👩🏽🧔🏽‍♂️ Launch Wonder Command v3.0 (Mama & Papa Voice + Gov/PDF Hub)" -ForegroundColor Yellow
    Write-Host "  [5] 🏙️ Launch Cities: Skylines II (with Purple City F8 Mod Installed)" -ForegroundColor Cyan
    Write-Host "  [6] 🎨 Run Headless Blender 5.2.3 CLI (Generate Cars, Flying Cars, District & Towers)" -ForegroundColor Blue
    Write-Host "  [7] 📂 Open Wonder World Project Folders in File Explorer" -ForegroundColor White
    Write-Host "  [Q] Exit" -ForegroundColor Gray
    Write-Host ""
    $choice = Read-Host "Select an option (1-7 or Q)"

    switch ($choice.ToUpper()) {
        "1" {
            Write-Host "Opening Open-World 3D Sandbox (Third-Person, Street Driving, Flying Cars & City Builder)..." -ForegroundColor Green
            Start-Process $OpenWorldHtml
            Start-Sleep -Seconds 2
        }
        "2" {
            Write-Host "Launching Unreal Engine 5.8 + Blender CLI + AI Copilot..." -ForegroundColor Cyan
            & "$UnrealProject\Scripts\Launch-Unreal-Blender-AICopilot.ps1"
            Start-Sleep -Seconds 3
        }
        "3" {
            Write-Host "Opening Mama's Wonder World Street Launcher..." -ForegroundColor Green
            Start-Process $GameHtml
            Start-Sleep -Seconds 2
        }
        "4" {
            Write-Host "Starting Wonder Command v3.0 Companion Server..." -ForegroundColor Yellow
            Start-Process powershell.exe -ArgumentList "-NoExit", "-ExecutionPolicy", "Bypass", "-File", "`"$UnrealProject\Scripts\Launch-WonderCommand.ps1`""
            Start-Sleep -Seconds 2
        }
        "5" {
            if (Test-Path $CS2Exe) {
                Write-Host "Launching Cities: Skylines II (Press F8 in-game for Purple City HUD)..." -ForegroundColor Cyan
                Start-Process -FilePath $CS2Exe -WorkingDirectory "D:\Games\Cities - Skylines II"
            } else {
                Write-Host "Cities2.exe not found at $CS2Exe" -ForegroundColor Red
            }
            Start-Sleep -Seconds 2
        }
        "6" {
            Write-Host "Running Headless Blender 5.2.3 CLI & Multi-Engine Ledger Sync..." -ForegroundColor Blue
            & "C:\Program Files\Blender Foundation\Blender 5.2\blender.exe" -b -P "$UnrealProject\Scripts\generate_open_world_sandbox_assets.py" -- --output "$UnrealProject\Assets\WonderWorld\OpenWorld"
            & "C:\Program Files\Blender Foundation\Blender 5.2\blender.exe" -b -P "$UnrealProject\Scripts\generate_metropolis_district.py" -- --output "$UnrealProject\Assets\WonderWorld\Environment"
            & py "$UnrealProject\WonderPipeline\sync_wonder_pipeline.py"
            Read-Host "Press Enter to return to the menu"
        }
        "7" {
            Start-Process explorer.exe $UnrealProject
            Start-Process explorer.exe $UniverseRepo
            Start-Sleep -Seconds 1
        }
        "Q" {
            exit
        }
    }
}
