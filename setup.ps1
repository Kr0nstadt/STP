$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

if (-not (Get-Command winget -ErrorAction SilentlyContinue)) {
    throw "winget is not available. Install App Installer from Microsoft Store first."
}

$packages = @(
    @{ Id = "Kitware.CMake"; Name = "CMake" },
    @{ Id = "BrechtSanders.WinLibs.POSIX.UCRT"; Name = "WinLibs/MinGW C++" }
)

foreach ($package in $packages) {
    Write-Host "Checking $($package.Name)..." -ForegroundColor Cyan
    winget list --id $package.Id --exact --source winget `
        --accept-source-agreements | Out-Null

    if ($LASTEXITCODE -eq 0) {
        Write-Host "$($package.Name) is already installed." -ForegroundColor Green
        continue
    }

    Write-Host "Installing $($package.Name)..." -ForegroundColor Yellow
    winget install --id $package.Id --exact --source winget `
        --accept-package-agreements --accept-source-agreements --silent

    if ($LASTEXITCODE -ne 0) {
        throw "Failed to install $($package.Name)."
    }
}

Write-Host "" 
Write-Host "Tools are installed." -ForegroundColor Green
Write-Host "Restart VS Code, open a new terminal and run: .\run.ps1"
