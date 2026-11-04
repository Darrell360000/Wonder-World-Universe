# Wonder Command v3.0 — One-Click Server & Browser Launcher (Mama & Papa Voice Hub)
$ErrorActionPreference = "Continue"
$ProjectRoot = "C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon"
Set-Location $ProjectRoot

Write-Host "==================================================================" -ForegroundColor Cyan
Write-Host " 🎩 WONDER COMMAND v3.0 // MAMA & PAPA VOICE + MULTI-ENGINE HUB" -ForegroundColor Cyan
Write-Host "==================================================================" -ForegroundColor Cyan
Write-Host "Starting Companion Server on http://127.0.0.1:8765/ ..." -ForegroundColor Yellow
Write-Host "Opening Wonder Command v3.0 in your browser..." -ForegroundColor Green

Start-Job -ScriptBlock {
    Start-Sleep -Seconds 2
    Start-Process "http://127.0.0.1:8765/"
} | Out-Null

& py "$ProjectRoot\Companion\wonder_command_companion.py" --host 0.0.0.0 --port 8765
