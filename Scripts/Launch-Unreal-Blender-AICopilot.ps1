# Mama's Wonder World // Unreal Engine 5.8 + Blender 5.2.3 CLI + AI Copilot Launcher
$ErrorActionPreference = "Continue"

function Find-FirstExisting {
    param([string[]]$Candidates)

    foreach ($candidate in $Candidates) {
        if (-not [string]::IsNullOrWhiteSpace($candidate) -and (Test-Path $candidate)) {
            return $candidate
        }
    }

    return $null
}

$ProjectRoot = Split-Path -Parent $PSScriptRoot
$ProjectRoot = (Resolve-Path $ProjectRoot).Path
$ProjectFile = Join-Path $ProjectRoot "WonderTowerTycoon.uproject"
$BlenderCandidates = @(
    "C:\Program Files\Blender Foundation\Blender 5.2\blender.exe",
    "C:\Program Files\Blender Foundation\Blender 4.5\blender.exe",
    "C:\Program Files\Blender Foundation\Blender\blender.exe",
    "$env:ProgramFiles\Blender Foundation\Blender 5.2\blender.exe",
    "$env:ProgramFiles\Blender Foundation\Blender 4.5\blender.exe",
    "$env:ProgramFiles\Blender Foundation\Blender\blender.exe"
)
$UnrealCandidates = @(
    "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe",
    "$env:ProgramFiles\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe",
    "$env:LOCALAPPDATA\Programs\Unreal Engine\Engine\Binaries\Win64\UnrealEditor.exe",
    "$env:ProgramW6432\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe"
)

$BlenderExe = Find-FirstExisting -Candidates $BlenderCandidates
$UnrealEditorExe = Find-FirstExisting -Candidates $UnrealCandidates
if (-not $UnrealEditorExe) {
    $UnrealEditorExe = Get-ChildItem -Path "$env:ProgramFiles\Epic Games" -Filter "UnrealEditor.exe" -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1 -ExpandProperty FullName
}

Set-Location $ProjectRoot

Write-Host "==============================================================================" -ForegroundColor Cyan
Write-Host " 🚀 MAMA'S WONDER WORLD // UNREAL ENGINE 5.8 + BLENDER 5.2.3 CLI + AI COPILOT" -ForegroundColor Cyan
Write-Host "==============================================================================" -ForegroundColor Cyan

if ($BlenderExe) {
    Write-Host "[1/4] Running Blender asset generation for the open-world city sandbox..." -ForegroundColor Yellow
    $AssetScript = Join-Path $ProjectRoot "Scripts\generate_open_world_sandbox_assets.py"
    if (Test-Path $AssetScript) {
        & $BlenderExe -b -P $AssetScript -- --output (Join-Path $ProjectRoot "Assets\WonderWorld\OpenWorld") | Out-Null
        Write-Host "      -> Exported WonderWorld_OpenWorld_Sandbox.{blend,fbx,glb}" -ForegroundColor Green
    } else {
        Write-Host "      -> Blender script not found: $AssetScript" -ForegroundColor Red
    }
} else {
    Write-Host "[1/4] Blender was not detected. Skipping asset generation." -ForegroundColor DarkYellow
}

Write-Host "[2/4] Syncing ledger and validating the project contracts..." -ForegroundColor Yellow
$SyncScript = Join-Path $ProjectRoot "WonderPipeline\sync_wonder_pipeline.py"
if (Test-Path $SyncScript) {
    & py $SyncScript
} else {
    Write-Host "      -> Sync script not found: $SyncScript" -ForegroundColor Red
}
$SubsystemScript = Join-Path $ProjectRoot "Scripts\validate_municipal_subsystems.py"
if (Test-Path $SubsystemScript) {
    & py $SubsystemScript
}

$PortActive = Get-NetTCPConnection -LocalPort 8765 -State Listen -ErrorAction SilentlyContinue
if (-not $PortActive) {
    Write-Host "[3/4] Starting Wonder Command v3.0 AI Copilot server on port 8765..." -ForegroundColor Yellow
    Start-Process powershell.exe -ArgumentList "-NoExit", "-ExecutionPolicy", "Bypass", "-File", "`"$ProjectRoot\Scripts\Launch-WonderCommand.ps1`""
} else {
    Write-Host "[3/4] Wonder Command v3.0 AI Copilot server is already active on port 8765." -ForegroundColor Green
}

if ($UnrealEditorExe -and (Test-Path $ProjectFile)) {
    Write-Host "[4/4] Launching Unreal Editor with WonderTowerTycoon.uproject..." -ForegroundColor Cyan
    Write-Host "      TIP: In the Unreal Editor Python console, run Scripts/open_world_city_sandbox_builder.py" -ForegroundColor Magenta
    Start-Process -FilePath $UnrealEditorExe -ArgumentList "`"$ProjectFile`" -dx11"
    Start-Process "http://127.0.0.1:8765/"
} else {
    if (-not $UnrealEditorExe) {
        Write-Host "UnrealEditor.exe was not found in the standard 5.8 install locations." -ForegroundColor Red
    }
    if (-not (Test-Path $ProjectFile)) {
        Write-Host "Missing project file: $ProjectFile" -ForegroundColor Red
    }
}
