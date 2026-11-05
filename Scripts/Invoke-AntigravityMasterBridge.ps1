# ==============================================================================
# GOOGLE ANTIGRAVITY - UNIFIED PIPELINE & WORKSPACE BRIDGE
# Root: C:\Users\Admin\.gemini\antigravity\scratch\Wonder-World-Universe
# ==============================================================================

$ErrorActionPreference = "SilentlyContinue"

# 1. Define Core Workspace & Drive Roots
$WorkspaceRoot = "C:\Users\Admin\.gemini\antigravity\scratch\Wonder-World-Universe"
$DevScratch    = "D:\"
$UE5Projects   = "E:\"
$WindowsF      = "F:\"
$BackupDrive   = "L:\"

# 2. Define Verified Application Dictionary
$PipelineApps = @{
    # Core & IDEs
    "Antigravity"       = "C:\Users\Admin\AppData\Local\Programs\antigravity\Antigravity.exe"
    "VSCode"            = "C:\Users\Admin\AppData\Local\Programs\Microsoft VS Code\Code.exe"
    "VSCodium"          = "C:\Users\Admin\AppData\Local\Programs\VSCodium\VSCodium.exe"
    "VS2022"            = "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe"
    "VS2026_18"         = "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\devenv.exe"
    "VSInstaller"       = "C:\Program Files (x86)\Microsoft Visual Studio\Installer\setup.exe"

    # Game Engines & Modding
    "UE5_8_Dir"         = "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64"
    "EpicLauncher"      = "C:\Program Files\Epic Games\Launcher\Portal\Binaries\Win64\EpicGamesLauncher.exe"
    "UnityHub"          = "C:\Program Files\WindowsApps\UnityTechnologies.UnityHub_3.22.5.65535_x64_2vrhnee42bhxm\app\Unity Hub.exe"
    "Godot4"            = "C:\Users\Admin\Pictures\Godot_v4.7.2-stable_win64.exe\Godot_v4.7.2-stable_win64.exe"
    "SummerEngine"      = "C:\Users\Admin\AppData\Local\SummerEngine\current\Summer.exe"
    "CitiesSkylines2"   = "D:\Games\Cities - Skylines II\Cities2.exe"

    # 3D, VFX & Rendering
    "Blender52"         = "C:\Program Files\Blender Foundation\Blender 5.2\blender.exe"
    "Cinema4D"          = "C:\Program Files\Maxon Cinema 4D R26\Cinema 4D.exe"
    "Cinema4DPython"    = "C:\Program Files\Maxon Cinema 4D R26\c4dpy.exe"
    "Cinema4DCmd"       = "C:\Program Files\Maxon Cinema 4D R26\Commandline.exe"
    "RTXRemix"          = "C:\Program Files\NVIDIA Corporation\RTX Remix\lightspeed.app.trex.bat"

    # AI Assistants & SDKs
    "GitHubCopilot"     = "C:\Users\Admin\AppData\Local\Programs\GitHub Copilot\github.exe"
    "CopilotCred"       = "C:\Users\Admin\AppData\Local\Programs\GitHub Copilot\git-credential-copilot.exe"
    "Gemini"            = "C:\Users\Admin\AppData\Local\Google\Gemini\Gemini.exe"
    "ChatGPT"           = "C:\Program Files\ChatGPT\ChatGPT.exe"

    # Audio & Music Production
    "StudioOne7"        = "C:\Program Files\PreSonus\Studio One 7\Studio One.exe"
    "StudioPro8"        = "C:\Program Files\Fender\Studio Pro 8\Studio Pro.exe"
    "StudioProRemote"   = "C:\Program Files\Fender\Studio Pro Remote\Studio Pro Remote.exe"
    "Moises"            = "C:\Users\Admin\AppData\Local\Programs\moises-desktop\Moises.exe"

    # Browsers, Media & Desktop Utilities
    "Chrome"            = "C:\Program Files\Google\Chrome\Application\chrome.exe"
    "Edge"              = "C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe"
    "Firefox"           = "C:\Program Files\Mozilla Firefox\firefox.exe"
    "Ace"               = "C:\Program Files\Ace\Ace\Application\ace.exe"
    "VLC"               = "C:\Program Files (x86)\VideoLAN\VLC\vlc.exe"
    "LivelyWallpaper"   = "C:\Users\Admin\AppData\Local\Programs\Lively Wallpaper\Lively.exe"
    "AcrobatDC"         = "C:\Program Files\Adobe\Acrobat DC\Acrobat\Acrobat.exe"
    "OneNote"           = "C:\Program Files\Microsoft Office\root\Office16\ONENOTE.EXE"
    "IObitUninstaller"  = "F:\IObit Uninstaller\IObitUninstaler.exe"
    "DriverBooster"     = "F:\iobit\DriverBoosterPortable.exe"
}

# 3. Inject Critical CLI Directories into Session PATH for Antigravity Agents
$BridgePaths = @(
    "C:\Users\Admin\AppData\Local\Programs\antigravity",
    "C:\Users\Admin\AppData\Local\Programs\Microsoft VS Code\bin",
    "C:\Users\Admin\AppData\Local\Programs\VSCodium\bin",
    "C:\Users\Admin\AppData\Local\Programs\GitHub Copilot",
    "C:\Program Files\Blender Foundation\Blender 5.2",
    "C:\Program Files\Maxon Cinema 4D R26",
    "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64",
    "C:\Users\Admin\Pictures\Godot_v4.7.2-stable_win64.exe"
)

foreach ($p in $BridgePaths) {
    if ((Test-Path $p) -and ($env:PATH -notlike "*$p*")) {
        $env:PATH += ";$p"
    }
}

function Start-BridgeApp {
    param([string]$AppName, [string]$Arguments = "")
    $exe = $PipelineApps[$AppName]
    if (Test-Path $exe) {
        Write-Host "[BRIDGE] Launching $AppName -> $exe" -ForegroundColor Cyan
        if ($Arguments) {
            Start-Process -FilePath $exe -ArgumentList $Arguments
        } else {
            Start-Process -FilePath $exe
        }
    } else {
        Write-Host "[WARN] Path not found for $AppName : $exe" -ForegroundColor Yellow
    }
}

function Start-ClaudeAndTikTok {
    Write-Host "[BRIDGE] Launching Claude App & TikTok Creator Hub..." -ForegroundColor Magenta
    Start-Process "explorer.exe" "shell:AppsFolder"
    if (Test-Path $PipelineApps["Chrome"]) {
        Start-Process $PipelineApps["Chrome"] "--app=https://www.tiktok.com/tiktokstudio"
    }
}

# 4. Interactive Command Menu
Clear-Host
Write-Host "================================================================" -ForegroundColor Green
Write-Host "   GOOGLE ANTIGRAVITY - WONDER WORLD UNIVERSE PIPELINE BRIDGE   " -ForegroundColor Green
Write-Host "================================================================" -ForegroundColor Green
Write-Host " [1] Launch Core Coding & AI Bridge (Antigravity, VS Code, VSCodium, Copilot, Gemini, ChatGPT)"
Write-Host " [2] Launch 3D & Game Engines (UE 5.8, Unity Hub/7, Godot 4.7.2, SummerEngine, Blender 5.2, C4D R26, RTX Remix)"
Write-Host " [3] Launch Audio Production Suite (Studio One 7, Studio Pro 8, Studio Pro Remote, Moises)"
Write-Host " [4] Launch TikTok Studio, Claude & Web Browsers (Chrome, Edge, Firefox, Ace)"
Write-Host " [5] Run Wonder-World Python -> UE5 Build (build_darrell_xgen_to_ue5.py)"
Write-Host " [6] Sync Wonder-World-Universe to GitHub (Sync-WonderWorldToGitHub.ps1)"
Write-Host " [7] Open Cities: Skylines II Mod Project in Visual Studio 2022/2026"
Write-Host " [A] LAUNCH FULL WORKSPACE CORE (Antigravity + VS Code + Blender + Studio One + AI)"
Write-Host " [Q] Quit"
Write-Host "================================================================" -ForegroundColor Green

$choice = Read-Host "Select a pipeline action"

switch ($choice.ToUpper()) {
    "1" {
        Start-BridgeApp "Antigravity" "`"$WorkspaceRoot`""
        Start-BridgeApp "VSCode" "`"$WorkspaceRoot`""
        Start-BridgeApp "VSCodium" "`"$WorkspaceRoot`""
        Start-BridgeApp "GitHubCopilot"
        Start-BridgeApp "Gemini"
        Start-BridgeApp "ChatGPT"
    }
    "2" {
        Start-BridgeApp "EpicLauncher"
        Start-BridgeApp "UnityHub"
        Start-BridgeApp "Godot4"
        Start-BridgeApp "SummerEngine"
        Start-BridgeApp "Blender52"
        Start-BridgeApp "Cinema4D"
        Start-BridgeApp "RTXRemix"
    }
    "3" {
        Start-BridgeApp "StudioOne7"
        Start-BridgeApp "StudioPro8"
        Start-BridgeApp "StudioProRemote"
        Start-BridgeApp "Moises"
    }
    "4" {
        Start-ClaudeAndTikTok
        Start-BridgeApp "Firefox"
        Start-BridgeApp "Ace"
    }
    "5" {
        Set-Location $WorkspaceRoot
        python .\build_darrell_xgen_to_ue5.py
    }
    "6" {
        Set-Location $WorkspaceRoot
        & .\Sync-WonderWorldToGitHub.ps1
    }
    "7" {
        Start-BridgeApp "VS2022" "`"D:\Games\Cities - Skylines II\WonderWorldCS2Mod.csproj`""
    }
    "A" {
        Start-BridgeApp "Antigravity" "`"$WorkspaceRoot`""
        Start-BridgeApp "VSCode" "`"$WorkspaceRoot`""
        Start-BridgeApp "Blender52"
        Start-BridgeApp "StudioOne7"
        Start-BridgeApp "Gemini"
        Start-BridgeApp "ChatGPT"
        Start-ClaudeAndTikTok
    }
    Default { Write-Host "Exiting Bridge." }
}
