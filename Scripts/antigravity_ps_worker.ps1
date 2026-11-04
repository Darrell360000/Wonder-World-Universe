$busPath = "C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon\WonderPipeline\06_Engine_Command_Logs\powershell_command_bus.jsonl"

Write-Host "[Antigravity Worker] Listening for live PowerShell commands..." -ForegroundColor Green

while ($true) {
    if (Test-Path $busPath) {
        $lines = Get-Content $busPath
        if ($lines.Count -gt 0) {
            foreach ($line in $lines) {
                if ([string]::IsNullOrWhiteSpace($line)) { continue }
                try {
                    $json = $line | ConvertFrom-Json
                    if ($json.command) {
                        Write-Host "[Executing] $($json.command)" -ForegroundColor Cyan
                        Invoke-Expression $json.command
                    }
                } catch {
                    Write-Error "Failed to execute: $line"
                }
            }
            Clear-Content $busPath
        }
    }
    Start-Sleep -Milliseconds 500
}