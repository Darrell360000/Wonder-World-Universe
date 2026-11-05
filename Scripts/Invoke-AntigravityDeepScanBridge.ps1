# ==============================================================================
# ANTIGRAVITY DEEP-SCAN BRIDGE: PROGRAMDATA + START MENU + MICROSOFT OFFICE 16
# Saves full recursive manifest to:
# C:\Users\Admin\.gemini\antigravity\scratch\Wonder-World-Universe\antigravity_full_system_bridge.json
# ==============================================================================

$ErrorActionPreference = "SilentlyContinue"
$WorkspaceDir = "C:\Users\Admin\.gemini\antigravity\scratch\Wonder-World-Universe"
$OutputJson   = Join-Path $WorkspaceDir "antigravity_full_system_bridge.json"
$WorkspaceFile = Join-Path $WorkspaceDir "WonderWorld_Full_ProgramData.code-workspace"

$WshShell = New-Object -ComObject WScript.Shell

Write-Host "[1/4] Scanning Start Menu Programs (All 88 Folders, Subfolders & Shortcuts)..." -ForegroundColor Cyan
$StartMenuRoot = "C:\ProgramData\Microsoft\Windows\Start Menu\Programs"
$StartMenuItems = @()

Get-ChildItem -Path $StartMenuRoot -Recurse -Force | ForEach-Object {
    $item = $_
    $targetExe = $null
    $arguments = $null

    if ($item.Extension -eq ".lnk") {
        $shortcut = $WshShell.CreateShortcut($item.FullName)
        $targetExe = $shortcut.TargetPath
        $arguments = $shortcut.Arguments
    }

    $StartMenuItems += [PSCustomObject]@{
        Name         = $item.Name
        RelativePath = $item.FullName.Substring($StartMenuRoot.Length + 1)
        FullPath     = $item.FullName
        IsDirectory  = $item.PSIsContainer
        Extension    = $item.Extension
        TargetExe    = $targetExe
        Arguments    = $arguments
        LastModified = $item.LastWriteTime.ToString("yyyy-MM-dd HH:mm:ss")
    }
}

Write-Host "[2/4] Scanning Microsoft Office 16 Root Executables & Tools..." -ForegroundColor Cyan
$OfficeRoot = "C:\Program Files\Microsoft Office\root\Office16"
$OfficeApps = @{}
$CoreOfficeExes = @(
    "WINWORD.EXE", "EXCEL.EXE", "POWERPNT.EXE", "OUTLOOK.EXE",
    "ONENOTE.EXE", "ONENOTEM.EXE", "MSACCESS.EXE", "MSPUB.EXE",
    "lync.exe", "ORGCHART.EXE", "OLCFG.EXE", "onenotecapture.exe"
)

if (Test-Path $OfficeRoot) {
    Get-ChildItem -Path $OfficeRoot -Filter "*.exe" -File | ForEach-Object {
        $OfficeApps[$_.Name] = $_.FullName
    }
}

Write-Host "[3/4] Scanning C:\ProgramData Top-Level Application Directories..." -ForegroundColor Cyan
$ProgramDataRoot = "C:\ProgramData"
$ProgramDataFolders = @()

Get-ChildItem -Path $ProgramDataRoot -Directory -Force | ForEach-Object {
    $ProgramDataFolders += [PSCustomObject]@{
        FolderName   = $_.Name
        FullPath     = $_.FullName
        LastModified = $_.LastWriteTime.ToString("yyyy-MM-dd HH:mm:ss")
    }
}

# Explicit Master Map of all 88 Start Menu ProgramData folders & shortcuts + Unity 6000.6 / 2022.3
$ExplicitStartMenuMap = @{
    # Folders in C:\ProgramData\Microsoft\Windows\Start Menu\Programs
    "7-Zip"                               = "$StartMenuRoot\7-Zip"
    "Accessibility"                       = "$StartMenuRoot\Accessibility"
    "Acoustica Mixcraft 7 (64-bit)"       = "$StartMenuRoot\Acoustica Mixcraft 7 (64-bit)"
    "Age of Empires II Definitive Edition"= "$StartMenuRoot\Age of Empires II Definitive Edition"
    "ASRock Utility"                      = "$StartMenuRoot\ASRock Utility"
    "ChatGPT"                             = "$StartMenuRoot\ChatGPT"
    "Cheat Engine"                        = "$StartMenuRoot\Cheat Engine"
    "Cities Skylines II Modern City"      = "$StartMenuRoot\Cities Skylines II Modern City"
    "Command and Conquer Remastered"      = "$StartMenuRoot\Command and Conquer Remastered Collection"
    "CPUID"                               = "$StartMenuRoot\CPUID"
    "Display Driver Uninstaller"          = "$StartMenuRoot\Display Driver Uninstaller"
    "Driver Booster 13"                   = "$StartMenuRoot\Driver Booster 13"
    "EaseUS Partition Master"             = "$StartMenuRoot\EaseUS Partition Master"
    "EPSON"                               = "$StartMenuRoot\EPSON"
    "EPSON Software"                      = "$StartMenuRoot\EPSON Software"
    "Games"                               = "$StartMenuRoot\Games"
    "Git"                                 = "$StartMenuRoot\Git"
    "Grand Theft Auto V Enhanced"         = "$StartMenuRoot\Grand Theft Auto V Enhanced"
    "GTA Vice City Nextgen Edition"       = "$StartMenuRoot\GTA Vice City Nextgen Edition"
    "Hasleo WinToUSB"                     = "$StartMenuRoot\Hasleo WinToUSB"
    "IObit Uninstaller"                   = "$StartMenuRoot\IObit Uninstaller"
    "K-Lite Codec Pack"                   = "$StartMenuRoot\K-Lite Codec Pack"
    "LibreOffice"                         = "$StartMenuRoot\LibreOffice"
    "Maintenance"                         = "$StartMenuRoot\Maintenance"
    "Marvels SpiderMan 2"                 = "$StartMenuRoot\Marvels SpiderMan 2"
    "Marvels SpiderMan Miles Morales"     = "$StartMenuRoot\Marvels SpiderMan Miles Morales"
    "Maxon"                               = "$StartMenuRoot\Maxon"
    "MediaInfo Lite"                      = "$StartMenuRoot\MediaInfo Lite"
    "Microsoft Office Tools"              = "$StartMenuRoot\Microsoft Office Tools"
    "Node.js"                             = "$StartMenuRoot\Node.js"
    "NVIDIA Corporation"                  = "$StartMenuRoot\NVIDIA Corporation"
    "NVIDIA FrameView"                    = "$StartMenuRoot\NVIDIA FrameView"
    "PCSX2"                               = "$StartMenuRoot\PCSX2"
    "PotPlayer"                           = "$StartMenuRoot\PotPlayer"
    "Samsung"                             = "$StartMenuRoot\Samsung"
    "SeaTools"                            = "$StartMenuRoot\SeaTools"
    "Smart Game Booster"                  = "$StartMenuRoot\Smart Game Booster"
    "Stardock"                            = "$StartMenuRoot\Stardock"
    "Startup"                             = "$StartMenuRoot\Startup"
    "TEAM R2R"                            = "$StartMenuRoot\TEAM R2R"
    "Tesseract-OCR"                       = "$StartMenuRoot\Tesseract-OCR"
    "Unity 2022.3.62f2"                   = "$StartMenuRoot\Unity 2022.3.62f2"
    "Unity 6000.6.0f1"                    = "$StartMenuRoot\Unity 6000.6.0f1"
    "Unreal Engine"                       = "$StartMenuRoot\Unreal Engine"
    "VideoLAN"                            = "$StartMenuRoot\VideoLAN"
    "Visual Studio"                       = "$StartMenuRoot\Visual Studio"
    "Visual Studio 2022"                  = "$StartMenuRoot\Visual Studio 2022"
    "Visual Studio 2026"                  = "$StartMenuRoot\Visual Studio 2026"
    "Windows Accessories"                 = "$StartMenuRoot\Windows Accessories"
    "Windows Kits"                        = "$StartMenuRoot\Windows Kits"
    "Windows PowerShell"                  = "$StartMenuRoot\Windows PowerShell"
    "Windows System"                      = "$StartMenuRoot\Windows System"
    "Windows Tools"                       = "$StartMenuRoot\Windows Tools"

    # Root Shortcuts (.lnk) in C:\ProgramData\Microsoft\Windows\Start Menu\Programs
    "Access"                              = "$StartMenuRoot\Access.lnk"
    "Ace"                                 = "$StartMenuRoot\Ace.lnk"
    "Adobe Acrobat"                       = "$StartMenuRoot\Adobe Acrobat.lnk"
    "Adobe Acrobat Distiller"             = "$StartMenuRoot\Adobe Acrobat Distiller.lnk"
    "Apple Software Update"               = "$StartMenuRoot\Apple Software Update.lnk"
    "Blend for Visual Studio"             = "$StartMenuRoot\Blend for Visual Studio.lnk"
    "Blend for Visual Studio Insiders"    = "$StartMenuRoot\Blend for Visual Studio Insiders.lnk"
    "Epic Games Launcher"                 = "$StartMenuRoot\Epic Games Launcher.lnk"
    "Excel"                               = "$StartMenuRoot\Excel.lnk"
    "Firefox"                             = "$StartMenuRoot\Firefox.lnk"
    "Firefox Private Browsing"            = "$StartMenuRoot\Firefox Private Browsing.lnk"
    "Google Chrome"                       = "$StartMenuRoot\Google Chrome.lnk"
    "Intel Driver & Support Assistant"    = "$StartMenuRoot\Intel Driver & Support Assistant.lnk"
    "IObit Uninstaller Shortcut"          = "$StartMenuRoot\IObit Uninstaller.lnk"
    "Microsoft Edge"                      = "$StartMenuRoot\Microsoft Edge.lnk"
    "NVIDIA RTX Remix"                    = "$StartMenuRoot\NVIDIA RTX Remix.lnk"
    "OneDrive"                            = "$StartMenuRoot\OneDrive.lnk"
    "OneNote"                             = "$StartMenuRoot\OneNote.lnk"
    "Outlook (classic)"                   = "$StartMenuRoot\Outlook (classic).lnk"
    "PowerPoint"                          = "$StartMenuRoot\PowerPoint.lnk"
    "Publisher"                           = "$StartMenuRoot\Publisher.lnk"
    "Rainmeter"                           = "$StartMenuRoot\Rainmeter.lnk"
    "RollerCoaster Tycoon World"          = "$StartMenuRoot\RollerCoaster Tycoon World.lnk"
    "Skype for Business"                  = "$StartMenuRoot\Skype for Business.lnk"
    "Sticky Notes (new)"                  = "$StartMenuRoot\Sticky Notes (new).lnk"
    "Studio One 7"                        = "$StartMenuRoot\Studio One 7.lnk"
    "Studio Pro 8"                        = "$StartMenuRoot\Studio Pro 8.lnk"
    "Studio Pro Remote"                   = "$StartMenuRoot\Studio Pro Remote.lnk"
    "Unity Hub"                           = "$StartMenuRoot\Unity Hub.lnk"
    "Visual Studio 2022 Shortcut"         = "$StartMenuRoot\Visual Studio 2022.lnk"
    "Visual Studio Installer"             = "$StartMenuRoot\Visual Studio Installer.lnk"
    "Visual Studio Shortcut"              = "$StartMenuRoot\Visual Studio.lnk"
    "Word"                                = "$StartMenuRoot\Word.lnk"
    "WSL"                                 = "$StartMenuRoot\WSL.lnk"
    "WSL Settings"                        = "$StartMenuRoot\WSL Settings.lnk"
}

$FullBridgePayload = [PSCustomObject]@{
    GeneratedAt            = (Get-Date).ToString("yyyy-MM-dd HH:mm:ss")
    AntigravityWorkspace   = $WorkspaceDir
    StartMenuRoot          = $StartMenuRoot
    Office16Root           = $OfficeRoot
    ProgramDataRoot        = $ProgramDataRoot
    Explicit88StartMenuMap = $ExplicitStartMenuMap
    Office16Executables    = $OfficeApps
    ProgramDataDirectories = $ProgramDataFolders
    RecursiveStartMenuTree = $StartMenuItems
}

Write-Host "[4/4] Writing Master JSON & Antigravity Workspace File..." -ForegroundColor Green
$FullBridgePayload | ConvertTo-Json -Depth 6 | Set-Content -Path $OutputJson -Encoding UTF8

# Build Multi-Root Workspace that mounts ProgramData, Start Menu, and Office16 directly in Antigravity
$WorkspaceConfig = @{
    folders = @(
        @{ name = "🌌 Wonder-World-Universe (Core)"; path = $WorkspaceDir },
        @{ name = "📂 Start Menu Programs (All 88 Folders & Subfolders)"; path = $StartMenuRoot },
        @{ name = "🏢 Microsoft Office 16 (Root & Tools)"; path = $OfficeRoot },
        @{ name = "🗄️ C:\ProgramData (Full Root)"; path = $ProgramDataRoot },
        @{ name = "🎮 UE5_PROJECTS (E:)"; path = "E:\" },
        @{ name = "🛠️ DEV_SCRATCH (D:)"; path = "D:\" },
        @{ name = "💾 BACKUP_2 (L:)"; path = "L:\" }
    )
    settings = @{
        "files.exclude" = @{
            "**/Microsoft/Windows Defender/**" = $true
            "**/Package Cache/**" = $true
        }
        "antigravity.bridge.manifestPath" = $OutputJson
        "antigravity.bridge.office16" = @{
            "Root"      = $OfficeRoot
            "Word"      = "$OfficeRoot\WINWORD.EXE"
            "Excel"     = "$OfficeRoot\EXCEL.EXE"
            "PowerPoint"= "$OfficeRoot\POWERPNT.EXE"
            "Outlook"   = "$OfficeRoot\OUTLOOK.EXE"
            "OneNote"   = "$OfficeRoot\ONENOTE.EXE"
            "Access"    = "$OfficeRoot\MSACCESS.EXE"
            "Publisher" = "$OfficeRoot\MSPUB.EXE"
            "SkypeBiz"  = "$OfficeRoot\lync.exe"
        }
        "antigravity.bridge.unityEditors" = @{
            "Unity_2022_3_62f2_Menu" = "$StartMenuRoot\Unity 2022.3.62f2"
            "Unity_6000_6_0f1_Menu"  = "$StartMenuRoot\Unity 6000.6.0f1"
            "UnityHub_Shortcut"      = "$StartMenuRoot\Unity Hub.lnk"
        }
        "antigravity.bridge.startMenu88" = $ExplicitStartMenuMap
    }
}

$WorkspaceConfig | ConvertTo-Json -Depth 6 | Set-Content -Path $WorkspaceFile -Encoding UTF8

Write-Host "✅ Complete! Bridged $($StartMenuItems.Count) recursive Start Menu items, $($OfficeApps.Count) Office16 executables, and $($ProgramDataFolders.Count) ProgramData directories." -ForegroundColor Green
Write-Host "📄 Manifest saved to: $OutputJson" -ForegroundColor Yellow
Write-Host "🚀 Workspace file saved to: $WorkspaceFile" -ForegroundColor Yellow
