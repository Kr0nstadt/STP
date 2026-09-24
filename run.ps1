param(
    [switch]$App
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Find-Tool {
    param(
        [string]$Command,
        [string[]]$Candidates
    )

    $availableCommand = Get-Command $Command -ErrorAction SilentlyContinue
    if ($availableCommand) {
        return $availableCommand.Source
    }

    foreach ($candidate in $Candidates) {
        if (Test-Path -LiteralPath $candidate) {
            return $candidate
        }
    }

    return $null
}

function Find-WinLibsTool {
    param([string]$FileName)

    $packageRoot = Join-Path $env:LOCALAPPDATA "Microsoft\WinGet\Packages"
    if (-not (Test-Path -LiteralPath $packageRoot)) {
        return $null
    }

    $packages = Get-ChildItem -LiteralPath $packageRoot -Directory `
        -Filter "BrechtSanders.WinLibs.POSIX.UCRT*" -ErrorAction SilentlyContinue
    foreach ($package in $packages) {
        $tool = Get-ChildItem -LiteralPath $package.FullName -Filter $FileName `
            -File -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($tool) {
            return $tool.FullName
        }
    }

    return $null
}

$cmake = Find-Tool "cmake" @(
    "C:\Program Files\CMake\bin\cmake.exe"
)
$ctest = Find-Tool "ctest" @(
    "C:\Program Files\CMake\bin\ctest.exe"
)
$gpp = Find-Tool "g++" @(
    "$env:LOCALAPPDATA\Microsoft\WinGet\Links\g++.exe"
)
if (-not $gpp) {
    $gpp = Find-WinLibsTool "g++.exe"
}

$make = Find-Tool "mingw32-make" @(
    "$env:LOCALAPPDATA\Microsoft\WinGet\Links\mingw32-make.exe"
)
if (-not $make) {
    $make = Find-WinLibsTool "mingw32-make.exe"
}

$windres = Find-Tool "windres" @(
    "$env:LOCALAPPDATA\Microsoft\WinGet\Links\windres.exe"
)
if (-not $windres) {
    $windres = Find-WinLibsTool "windres.exe"
}

if (-not $cmake -or -not $ctest -or -not $gpp -or -not $make -or -not $windres) {
    Write-Host "Build tools were not found." -ForegroundColor Red
    Write-Host "Run .\setup.ps1 once, restart VS Code and try again."
    exit 1
}

# CMake cache files treat backslashes as escape characters. Forward slashes
# work on Windows and keep paths such as C:\Users from becoming invalid \U escapes.
$gppForCmake = $gpp.Replace("\", "/")
$makeForCmake = $make.Replace("\", "/")
$windresForCmake = $windres.Replace("\", "/")

# Test executables need the MinGW runtime DLLs during Google Test discovery.
$toolchainBin = Split-Path -Parent $gpp
$env:PATH = "$toolchainBin;$env:PATH"

$repository = Split-Path -Parent $MyInvocation.MyCommand.Path

# MinGW build tools can corrupt Cyrillic source paths. A directory junction
# gives CMake an ASCII-only path while all files remain in the repository.
$linkRoot = Join-Path $env:LOCALAPPDATA "CMakeSourceLinks"
$sourceLink = Join-Path $linkRoot "STP"
New-Item -ItemType Directory -Path $linkRoot -Force | Out-Null

if (Test-Path -LiteralPath $sourceLink) {
    $linkItem = Get-Item -LiteralPath $sourceLink -Force
    $existingTarget = [string]$linkItem.Target
    $expectedTarget = [System.IO.Path]::GetFullPath($repository).TrimEnd("\")
    $actualTarget = [System.IO.Path]::GetFullPath($existingTarget).TrimEnd("\")
    if (-not $actualTarget.Equals(
            $expectedTarget,
            [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "$sourceLink already points to another directory."
    }
}
else {
    New-Item -ItemType Junction -Path $sourceLink -Target $repository | Out-Null
}

$branchName = (& git -C $repository branch --show-current).Trim()
if (-not $branchName) {
    $branchName = "detached"
}
$safeBranchName = $branchName -replace "[^A-Za-z0-9._-]", "_"
$buildRoot = Join-Path $env:LOCALAPPDATA "CMakeBuilds\STP"
$buildDirectory = Join-Path $buildRoot $safeBranchName
New-Item -ItemType Directory -Path $buildDirectory -Force | Out-Null

$sourceForCmake = $sourceLink.Replace("\", "/")
$buildForCmake = $buildDirectory.Replace("\", "/")

Push-Location $sourceLink

try {
    $configureArguments = @(
        "--fresh",
        "-S", $sourceForCmake,
        "-B", $buildForCmake,
        "-G", "MinGW Makefiles",
        "-DCMAKE_BUILD_TYPE=Debug",
        "-DCMAKE_MAKE_PROGRAM=$makeForCmake",
        "-DCMAKE_CXX_COMPILER=$gppForCmake",
        "-DCMAKE_RC_COMPILER=$windresForCmake"
    )

    $isTaskManager = Test-Path -LiteralPath "src\mainwindow.cpp"
    if ($isTaskManager) {
        if ($App) {
            $configureArguments += "-DBUILD_GUI=ON"
        }
        else {
            $configureArguments += "-DBUILD_GUI=OFF"
        }
        $configureArguments += "-DBUILD_TESTING=ON"
    }
    elseif ($App) {
        throw "The -App option is available only in the lab3 branch."
    }

    Write-Host "Configuring project..." -ForegroundColor Cyan
    & $cmake @configureArguments
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed."
    }

    Write-Host "Building project..." -ForegroundColor Cyan
    & $cmake --build $buildForCmake --parallel
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed."
    }

    Write-Host "Running tests..." -ForegroundColor Cyan
    & $ctest --test-dir $buildForCmake --output-on-failure
    if ($LASTEXITCODE -ne 0) {
        throw "Tests failed."
    }

    Write-Host "Build and tests completed successfully." -ForegroundColor Green

    if ($App) {
        $application = Join-Path $buildDirectory "TaskManager_FLTK.exe"
        if (-not (Test-Path -LiteralPath $application)) {
            $application = Join-Path $buildDirectory "Debug\TaskManager_FLTK.exe"
        }
        if (-not (Test-Path -LiteralPath $application)) {
            throw "TaskManager_FLTK executable was not found after the build."
        }

        Start-Process -FilePath $application
    }
}
finally {
    Pop-Location
}
