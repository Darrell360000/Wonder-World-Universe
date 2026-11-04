Write-Host "=== Syncing Mama's Wonder World Universe to GitHub ===" -ForegroundColor Magenta
Set-Location $PSScriptRoot

if (-not (Test-Path ".git")) {
    git init
    git branch -M main
}

# Copy instructions so both GitHub Copilot and Google Antigravity read the same rules
New-Item -ItemType Directory -Force -Path ".github" | Out-Null
if (Test-Path "wonder_world_antigravity_github_bridge.md") {
    Copy-Item "wonder_world_antigravity_github_bridge.md" ".github\copilot-instructions.md" -Force
    Copy-Item "wonder_world_antigravity_github_bridge.md" "ANTIGRAVITY.md" -Force
}

git add -A
git status --short

$commitMsg = "Sync Darrell X-Gen Blender-to-UE5 asset & Wonder World Universe via Antigravity + Copilot"
git commit -m $commitMsg 2>$null

$remotes = git remote
if ($remotes -contains "origin") {
    Write-Host "Pushing to GitHub origin/main..." -ForegroundColor Cyan
    git push -u origin main
} else {
    $ghInstalled = Get-Command gh -ErrorAction SilentlyContinue
    if ($ghInstalled) {
        Write-Host "GitHub CLI (gh) detected! Creating or pushing to Wonder-World-Universe..." -ForegroundColor Cyan
        gh repo create Wonder-World-Universe --public --source=. --remote=origin --push
    } else {
        $repoUrl = Read-Host "Enter your GitHub Repo URL (e.g. https://github.com/YOUR_USERNAME/Wonder-World-Universe.git) or press Enter to skip"
        if ($repoUrl) {
            git remote add origin $repoUrl
            git push -u origin main
        }
    }
}
Write-Host "=== Sync Complete! ===" -ForegroundColor Green
