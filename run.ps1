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

$cmake = Find-Tool "cmake" @(
    "C:\Program Files\CMake\bin\cmake.exe"
)
$ctest = Find-Tool "ctest" @(
    "C:\Program Files\CMake\bin\ctest.exe"
)
$ninja = Find-Tool "ninja" @(
    "$env:LOCALAPPDATA\Microsoft\WinGet\Links\ninja.exe",
    "C:\Program Files\Ninja\ninja.exe"
)
$clang = Find-Tool "clang++" @(
    "C:\Program Files\LLVM\bin\clang++.exe"
)

if (-not $cmake -or -not $ctest -or -not $ninja -or -not $clang) {
    Write-Host "Build tools were not found." -ForegroundColor Red
    Write-Host "Run .\setup.ps1 once, restart VS Code and try again."
    exit 1
}

$repository = Split-Path -Parent $MyInvocation.MyCommand.Path
Push-Location $repository

try {
    $configureArguments = @(
        "--fresh",
        "-S", ".",
        "-B", "build",
        "-G", "Ninja",
        "-DCMAKE_BUILD_TYPE=Debug",
        "-DCMAKE_MAKE_PROGRAM=$ninja",
        "-DCMAKE_CXX_COMPILER=$clang"
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
    & $cmake --build build --parallel
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed."
    }

    Write-Host "Running tests..." -ForegroundColor Cyan
    & $ctest --test-dir build --output-on-failure
    if ($LASTEXITCODE -ne 0) {
        throw "Tests failed."
    }

    Write-Host "Build and tests completed successfully." -ForegroundColor Green

    if ($App) {
        $application = Join-Path $repository "build\TaskManager_FLTK.exe"
        if (-not (Test-Path -LiteralPath $application)) {
            $application = Join-Path $repository "build\Debug\TaskManager_FLTK.exe"
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
