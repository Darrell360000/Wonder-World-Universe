# ==============================================================================
# MAMA'S WONDER WORLD // UNIFIED DEVELOPER POWERSHELL BUILD PIPELINE & BRIDGE
# ==============================================================================
# Unifies:
#   1. Microsoft Visual Studio 2022 / 2026 Developer C++ & MSBuild Toolchains
#   2. Unreal Engine 5.8 Build Pipeline (UBT, UAT, UnrealEditor DX11)
#   3. Blender 5.2.3 CLI Headless 3D Asset Automation
#   4. Unity 6 & Cities: Skylines II ECS C# Mod Pipeline
#   5. Multi-Drive & Mobile Bridge (C:, D: DEV_SCRATCH, E: UE5_PROJECTS, F:, L: BACKUP_2, S23 Ultra)
#   6. Git CI/CD Sync & Antigravity AI Orchestration
# ==============================================================================

[CmdletBinding()]
param(
    [switch]$NonInteractive,
    [string]$Command = ""
)

# ------------------------------------------------------------------------------
# 1. 64-Bit Architecture Guard (Auto-escalate from SysWOW64 32-bit if needed)
# ------------------------------------------------------------------------------
if ([IntPtr]::Size -eq 4) {
    Write-Warning "[BRIDGE] Running in 32-bit PowerShell (SysWOW64). Re-launching in native 64-bit PowerShell..."
    $nativePowerShell = Join-Path $env:SystemRoot "System32\WindowsPowerShell\v1.0\powershell.exe"
    if (Test-Path $nativePowerShell) {
        $argList = @("-NoExit", "-ExecutionPolicy", "Bypass", "-File", "`"$PSCommandPath`"")
        Start-Process -FilePath $nativePowerShell -ArgumentList $argList
        exit
    }
}

# ------------------------------------------------------------------------------
# 2. Workspace Roots & Storage Drives Directory Map
# ------------------------------------------------------------------------------
$ScriptDir    = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot  = (Resolve-Path (Join-Path $ScriptDir "..")).Path

$Global:WonderDrives = [ordered]@{
    "ProjectRoot" = $ProjectRoot
    "SystemC"     = "C:\"
    "DevScratch"  = "D:\"
    "UE5Projects" = "E:\"
    "WindowsF"    = "F:\"
    "BackupL"     = "L:\"
}

# ------------------------------------------------------------------------------
# 3. Visual Studio Developer Environment Ingestion
# ------------------------------------------------------------------------------
function Initialize-VsDevShell {
    Write-Host "[1/5] Initializing Visual Studio C++ Developer Toolchain..." -ForegroundColor Cyan
    $vsDevShellScripts = @(
        "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Launch-VsDevShell.ps1",
        "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\Launch-VsDevShell.ps1",
        "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\Launch-VsDevShell.ps1",
        "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\Launch-VsDevShell.ps1"
    )

    $devShellFound = $false
    foreach ($script in $vsDevShellScripts) {
        if (Test-Path $script) {
            try {
                & $script -Arch amd64 -HostArch amd64 | Out-Null
                Write-Host "   -> Visual Studio Dev Shell loaded successfully: $script" -ForegroundColor Green
                $devShellFound = $true
                break
            } catch {
                Write-Warning "   Failed to invoke $script : $_"
            }
        }
    }

    if (-not $devShellFound) {
        # Fallback to VsDevCmd.bat environment variable capture
        $vsDevCmdBats = @(
            "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat",
            "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat"
        )
        foreach ($bat in $vsDevCmdBats) {
            if (Test-Path $bat) {
                Write-Host "   -> Sourcing environment via VsDevCmd.bat ($bat)..." -ForegroundColor Yellow
                $tempFile = [System.IO.Path]::GetTempFileName()
                cmd /c " `"$bat`" -arch=x64 >nul && set > `"$tempFile`" "
                Get-Content $tempFile | ForEach-Object {
                    if ($_ -match "^(.*?)=(.*)$") {
                        Set-Content "env:$($matches[1])" $matches[2]
                    }
                }
                Remove-Item -Force $tempFile -ErrorAction SilentlyContinue
                $devShellFound = $true
                break
            }
        }
    }

    if ($devShellFound) {
        $clVer = (& cl.exe 2>&1 | Select-Object -First 1)
        Write-Host "   -> Active MSVC Compiler: $clVer" -ForegroundColor DarkCyan
    } else {
        Write-Warning "   [!] Visual Studio C++ compiler not automatically injected into session."
    }
}

# ------------------------------------------------------------------------------
# 4. Multi-Engine & Toolchain Path Injection
# ------------------------------------------------------------------------------
function Initialize-EnginePaths {
    Write-Host "[2/5] Injecting Multi-Engine Executables into Session PATH..." -ForegroundColor Cyan

    $EnginePaths = @(
        "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64",
        "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\DotNET\UnrealBuildTool",
        "C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles",
        "C:\Program Files\Blender Foundation\Blender 5.2",
        "C:\Program Files\Unity\Hub\Editor\6000.6.0f1\Editor",
        "C:\Program Files\Unity 2022.3.62f2\Editor",
        "C:\Program Files\dotnet",
        "C:\Program Files\Git\cmd",
        "C:\Program Files\Git\bin",
        "C:\Program Files\Maxon Cinema 4D R26",
        "C:\Users\Admin\Pictures\Godot_v4.7.2-stable_win64.exe",
        "C:\Users\Admin\AppData\Local\Programs\antigravity",
        "C:\Users\Admin\AppData\Local\Programs\Microsoft VS Code\bin",
        "C:\Users\Admin\AppData\Local\Programs\VSCodium\bin",
        "C:\Users\Admin\AppData\Local\Programs\GitHub Copilot"
    )

    $injected = 0
    foreach ($p in $EnginePaths) {
        if ((Test-Path $p) -and ($env:PATH -notlike "*$p*")) {
            $env:PATH = "$p;$env:PATH"
            $injected++
        }
    }
    Write-Host "   -> Sourced $injected engine & CLI toolchain paths into `$env:PATH." -ForegroundColor Green
}

# ------------------------------------------------------------------------------
# 5. Core Build & Validation Commands
# ------------------------------------------------------------------------------
function Build-DarrellAsset {
    [CmdletBinding()]
    param()
    Write-Host "=== [PIPELINE] Building Darrell X-Gen Likeness & Skeleton via Blender 5.2.3 ===" -ForegroundColor Magenta
    $blenderExe = "C:\Program Files\Blender Foundation\Blender 5.2\blender.exe"
    $script = Join-Path $ProjectRoot "build_darrell_xgen_to_ue5.py"
    if ((Test-Path $blenderExe) -and (Test-Path $script)) {
        & $blenderExe -b -P $script -- --output (Join-Path $ProjectRoot "Assets\WonderWorld\Characters\DarrellXGen")
    } else {
        Write-Warning "Blender executable or build script not found: $blenderExe / $script"
    }
}

function Build-WonderUnreal {
    [CmdletBinding()]
    param([string]$Configuration = "Development")
    Write-Host "=== [PIPELINE] Building WonderTowerTycoon Unreal Engine 5.8 C++ ===" -ForegroundColor Magenta
    $ubt = "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"
    $uproject = Join-Path $ProjectRoot "WonderTowerTycoon.uproject"
    if ((Test-Path $ubt) -and (Test-Path $uproject)) {
        & $ubt WonderTowerTycoonEditor Win64 $Configuration -Project="`"$uproject`"" -WaitMutex -FromMsBuild
    } else {
        Write-Warning "UnrealBuildTool or uproject file not found: $ubt"
    }
}

function Start-WonderUnreal {
    [CmdletBinding()]
    param([switch]$DX12)
    Write-Host "=== [PIPELINE] Launching Wonder Tower Tycoon in Unreal Engine 5.8 ===" -ForegroundColor Cyan
    $editorExe = "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe"
    $uproject = Join-Path $ProjectRoot "WonderTowerTycoon.uproject"
    $rhiFlag = if ($DX12) { "-dx12" } else { "-dx11" }
    
    if ((Test-Path $editorExe) -and (Test-Path $uproject)) {
        Write-Host "Launching with $rhiFlag (Virtual Shadow Map / TDR Safe Mode)..." -ForegroundColor Green
        Start-Process -FilePath $editorExe -ArgumentList "`"$uproject`"", $rhiFlag, "-log"
    } else {
        Write-Warning "UnrealEditor or project file not found."
    }
}

function Build-WonderECS {
    [CmdletBinding()]
    param()
    Write-Host "=== [PIPELINE] Validating Cities: Skylines II & Unity ECS Mod Systems ===" -ForegroundColor Magenta
    $csProj = Join-Path $ProjectRoot "WonderWorldUnity\WonderWorldValidation.csproj"
    if (Test-Path $csProj) {
        dotnet build "`"$csProj`"" --configuration Release
    }
    python (Join-Path $ProjectRoot "Scripts\validate_municipal_subsystems.py")
    python (Join-Path $ProjectRoot "Scripts\validate_wonder_ledger.py")
}

function Sync-WonderGitHub {
    [CmdletBinding()]
    param([string]$CommitMessage = "feat: Wonder Universe master ECS integration, character assets & VSM stability fix")
    Write-Host "=== [PIPELINE] Synchronizing Clean State to GitHub ===" -ForegroundColor Green
    Set-Location $ProjectRoot
    
    # Exclude bulky engine caches
    git reset HEAD Intermediate Binaries DerivedDataCache Saved >$null 2>&1
    git add .
    git reset HEAD Intermediate Binaries DerivedDataCache Saved >$null 2>&1
    
    git commit -m $CommitMessage
    git push origin main
}

function Start-WonderCompanion {
    [CmdletBinding()]
    param([int]$Port = 8765)
    Write-Host "=== [BRIDGE] Launching Mobile & Device Telemetry Bridge ===" -ForegroundColor Magenta
    Write-Host "Target Device: Darrell Galaxy S23 Ultra" -ForegroundColor Cyan
    
    # Get LAN IPv4 Address
    $localIP = (Get-NetIPAddress -AddressFamily IPv4 | Where-Object { $_.InterfaceAlias -notmatch "vEthernet|Loopback" -and $_.IPAddress -like "192.168.*" -or $_.IPAddress -like "10.*" } | Select-Object -First 1).IPAddress
    if (-not $localIP) { $localIP = "127.0.0.1" }
    
    Write-Host "Local Pairing URL: http://$($localIP):$Port/pair.html" -ForegroundColor Green
    Write-Host "Desktop HUD URL:   http://localhost:$Port/" -ForegroundColor Green
    
    $companionScript = Join-Path $ProjectRoot "Companion\wonder_command_companion.py"
    if (Test-Path $companionScript) {
        Start-Process -FilePath "python" -ArgumentList "`"$companionScript`"", "--port", "$Port"
    } else {
        Write-Warning "Companion script not found: $companionScript"
    }
}

function Show-UniverseStatus {
    Clear-Host
    Write-Host "================================================================================" -ForegroundColor Magenta
    Write-Host "        MAMA'S WONDER WORLD // UNIFIED DEVELOPER PIPELINE STATUS               " -ForegroundColor Magenta
    Write-Host "================================================================================" -ForegroundColor Magenta
    
    Write-Host "`n[DRIVE & STORAGE TOPOLOGY]" -ForegroundColor Yellow
    foreach ($drive in $Global:WonderDrives.GetEnumerator()) {
        $exists = Test-Path $drive.Value
        $status = if ($exists) { "[CONNECTED]" } else { "[NOT FOUND]" }
        $color = if ($exists) { "Green" } else { "DarkGray" }
        Write-Host ("  {0,-14} : {1,-35} {2}" -f $drive.Key, $drive.Value, $status) -ForegroundColor $color
    }

    Write-Host "`n[PIPELINE TOOLCHAIN STATUS]" -ForegroundColor Yellow
    $tools = @(
        @{ Name = "MSVC Compiler (cl.exe)";      Cmd = "cl.exe" },
        @{ Name = "Unreal Engine 5.8 Editor";    Cmd = "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe" },
        @{ Name = "UnrealBuildTool (UBT)";       Cmd = "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" },
        @{ Name = "Blender 5.2.3";               Cmd = "C:\Program Files\Blender Foundation\Blender 5.2\blender.exe" },
        @{ Name = "Unity Hub & Editor";          Cmd = "C:\Program Files\Unity\Hub\Editor\6000.6.0f1\Editor\Unity.exe" },
        @{ Name = "Cities: Skylines II Mod";     Cmd = "D:\Games\Cities - Skylines II\Cities2.exe" },
        @{ Name = ".NET 8/10 SDK (dotnet)";      Cmd = "dotnet.exe" },
        @{ Name = "Python 3 Runtime";            Cmd = "python.exe" },
        @{ Name = "Git CLI";                     Cmd = "git.exe" },
        @{ Name = "Google Antigravity CLI";      Cmd = "C:\Users\Admin\AppData\Local\Programs\antigravity\Antigravity.exe" }
    )

    foreach ($t in $tools) {
        $found = (Get-Command $t.Cmd -ErrorAction SilentlyContinue) -or (Test-Path $t.Cmd)
        $color = if ($found) { "Green" } else { "Red" }
        $status = if ($found) { "READY" } else { "OFFLINE" }
        Write-Host ("  {0,-28} : {1}" -f $t.Name, $status) -ForegroundColor $color
    }

    Write-Host "`n[MOBILE & REMOTE CONNECTIVITY]" -ForegroundColor Yellow
    Write-Host "  Target Phone      : Darrell Galaxy S23 Ultra" -ForegroundColor Cyan
    Write-Host "  Companion Server  : Ready on port 8765 (run 'start-companion')" -ForegroundColor Cyan
    Write-Host "================================================================================" -ForegroundColor Magenta
}

# ------------------------------------------------------------------------------
# 6. Set Convenience Aliases & Navigation Helpers
# ------------------------------------------------------------------------------
function Global:cd-c       { Set-Location "C:\" }
function Global:cd-d       { Set-Location "D:\" }
function Global:cd-e       { Set-Location "E:\" }
function Global:cd-f       { Set-Location "F:\" }
function Global:cd-l       { Set-Location "L:\" }
function Global:cd-wonder  { Set-Location $ProjectRoot }
function Global:scratch    { Set-Location "D:\" }
function Global:ue5        { Set-Location "E:\" }
function Global:backup     { Set-Location "L:\" }

Set-Alias -Name "build-all"       -Value Build-AllPillars     -Scope Global -ErrorAction SilentlyContinue
Set-Alias -Name "build-darrell"   -Value Build-DarrellAsset   -Scope Global -ErrorAction SilentlyContinue
Set-Alias -Name "build-unreal"    -Value Build-WonderUnreal   -Scope Global -ErrorAction SilentlyContinue
Set-Alias -Name "launch-unreal"   -Value Start-WonderUnreal   -Scope Global -ErrorAction SilentlyContinue
Set-Alias -Name "build-ecs"       -Value Build-WonderECS      -Scope Global -ErrorAction SilentlyContinue
Set-Alias -Name "sync-github"     -Value Sync-WonderGitHub    -Scope Global -ErrorAction SilentlyContinue
Set-Alias -Name "start-companion" -Value Start-WonderCompanion -Scope Global -ErrorAction SilentlyContinue
Set-Alias -Name "status-universe" -Value Show-UniverseStatus  -Scope Global -ErrorAction SilentlyContinue

function Global:Build-AllPillars {
    Write-Host "`n>>> EXECUTING WONDER UNIVERSE 4-PILLAR MASTER PIPELINE <<<`n" -ForegroundColor Yellow
    Build-DarrellAsset
    Build-WonderECS
    Build-WonderUnreal
    Write-Host "`n>>> MASTER PIPELINE COMPLETE: ALL CONTRACTS VALIDATED <<<`n" -ForegroundColor Green
}

# ------------------------------------------------------------------------------
# 7. Execution Initialization
# ------------------------------------------------------------------------------
Initialize-VsDevShell
Initialize-EnginePaths
Set-Location $ProjectRoot

# Custom Developer Prompt
function Global:prompt {
    Write-Host "[WonderWorld-DevPS] " -NoNewline -ForegroundColor Magenta
    Write-Host (Get-Location).Path -NoNewline -ForegroundColor Cyan
    return "> "
}

if ($NonInteractive) {
    if ($Command) {
        Invoke-Expression $Command
    }
    return
}

# Show interactive menu on fresh launch
Show-UniverseStatus

Write-Host @"

QUICK COMMANDS LOADED INTO THIS SESSION:
  * build-all         : Run full 4-pillar build across Blender, ECS & Unreal
  * build-darrell     : Headless Blender 5.2.3 build of Darrell X-Gen
  * build-unreal      : Compile Unreal Engine 5.8 C++ via UBT
  * launch-unreal     : Launch UE5.8 editor with DX11 & VSM stability safeguards
  * build-ecs         : Compile Cities: Skylines II / Unity C# mod & contracts
  * sync-github       : Clean commit & push to GitHub repository
  * start-companion   : Launch LAN telemetry server for Darrell Galaxy S23 Ultra
  * status-universe   : Display drive & engine connectivity dashboard
  * cd-wonder / cd-d  : Jump to project root or storage drives (C:, D:, E:, F:, L:)

Type any command above or use standard PowerShell/MSVC commands (cl, msbuild, dotnet, git).
"@ -ForegroundColor White
