# remake.ps1 — close mGBA, delete build outputs, rebuild, launch ROM in mGBA
$ErrorActionPreference = "Stop"

$RepoRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $RepoRoot

# Close any running mGBA instances so the ROM can be rebuilt and relaunched cleanly
$MgbaProcessNames = @("mGBA", "mgba", "mgba-qt")
$closed = $false
foreach ($name in $MgbaProcessNames) {
    $procs = Get-Process -Name $name -ErrorAction SilentlyContinue
    if ($procs) {
        $procs | Stop-Process -Force
        $closed = $true
    }
}
if ($closed) {
    Write-Host "Closed running mGBA"
    Start-Sleep -Milliseconds 300
}

$Elf = Join-Path $RepoRoot "Skiii-gba.elf"
$Gba = Join-Path $RepoRoot "Skiii-gba.gba"

foreach ($f in @($Elf, $Gba)) {
    if (Test-Path $f) {
        Remove-Item -Force $f
        Write-Host "Deleted $(Split-Path -Leaf $f)"
    }
}

Write-Host "Running make..."
& make
if ($LASTEXITCODE -ne 0) {
    Write-Error "make failed with exit code $LASTEXITCODE"
    exit $LASTEXITCODE
}

if (-not (Test-Path $Gba)) {
    Write-Error "Build finished but Skiii-gba.gba was not found."
    exit 1
}

# Resolve mGBA executable
$MgbaCandidates = @(
    "mgba-qt.exe",
    "mgba.exe",
    "${env:ProgramFiles}\mGBA\mGBA.exe",
    "${env:ProgramFiles(x86)}\mGBA\mGBA.exe",
    "$env:LOCALAPPDATA\Programs\mGBA\mGBA.exe"
)

$Mgba = $null
foreach ($c in $MgbaCandidates) {
    if ($c -notmatch '[\\/]') {
        $cmd = Get-Command $c -ErrorAction SilentlyContinue
        if ($cmd) {
            $Mgba = $cmd.Source
            break
        }
    } elseif (Test-Path $c) {
        $Mgba = $c
        break
    }
}

if (-not $Mgba) {
    Write-Error "Could not find mGBA. Install it or add mgba.exe / mGBA.exe to PATH."
    exit 1
}

Write-Host "Launching mGBA with Skiii-gba.gba..."
Start-Process -FilePath $Mgba -ArgumentList "`"$Gba`"" -WorkingDirectory $RepoRoot
