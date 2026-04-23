#Requires -Version 5.0
<#
  Downloads FireDaemon's OpenSSL 3.x MSVC prebuild (same as cmake/downloadOpensslWindows.cmake)
  when vendor/openssl-firedaemon/x64 is missing. Set OPENSSL_ROOT_DIR to the printed x64 path
  (CMake handles this automatically if you do not set it).

  Run from the repo root:
    powershell -ExecutionPolicy Bypass -File scripts/download_openssl_windows.ps1
#>
$ErrorActionPreference = 'Stop'

$Version = '3.5.6'
$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$BaseDir = Join-Path $RepoRoot "vendor\openssl-firedaemon"
$X64 = Join-Path $BaseDir "x64"
$ZipName = "openssl-$Version.zip"
$ZipPath = Join-Path $BaseDir $ZipName
$Url = "https://download.firedaemon.com/FireDaemon-OpenSSL/$ZipName"
$Marker = Join-Path $X64 "include\openssl\opensslv.h"

if (Test-Path -LiteralPath $Marker) {
    Write-Host "OpenSSL $Version (x64) already present. OPENSSL_ROOT_DIR for MSVC:"
    Write-Host $X64
    exit 0
}

New-Item -ItemType Directory -Force -Path $BaseDir | Out-Null

if (-not (Test-Path -LiteralPath $ZipPath)) {
    Write-Host "Downloading $Url ..."
    Invoke-WebRequest -Uri $Url -OutFile $ZipPath
}

if (-not (Test-Path -LiteralPath $Marker)) {
    Write-Host "Extracting to $BaseDir ..."
    Expand-Archive -LiteralPath $ZipPath -DestinationPath $BaseDir -Force
}

if (-not (Test-Path -LiteralPath $Marker)) {
    Write-Error "Expected $Marker after extract. Remove vendor\openssl-firedaemon and retry, or set OPENSSL_ROOT_DIR to a full install."
    exit 1
}

Write-Host "Done. For MSVC, use:"
Write-Host "  -D OPENSSL_ROOT_DIR=`"$X64`""
Write-Host "or run CMake without OPENSSL_ROOT_DIR; the project will set this under vendor/ when using MSVC."
