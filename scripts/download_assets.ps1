# download_assets.ps1
# This script downloads a lightweight default piano SF2 soundfont for Joychord's PIANO tier.

$ErrorActionPreference = "Stop"

$assetsDir = Join-Path $PSScriptRoot "..\assets"
$targetFile = Join-Path $assetsDir "default_piano.sf2"

# Ensure assets directory exists
if (-not (Test-Path $assetsDir)) {
    Write-Host "Creating assets directory..."
    New-Item -ItemType Directory -Force -Path $assetsDir | Out-Null
}

if (Test-Path $targetFile) {
    Write-Host "Asset already exists: $targetFile"
    Write-Host "Skipping download."
    exit 0
}

# Source: TimGM6mb or ChoriumRevA (Requires a direct link which can be prone to change).
# As a stable alternative, we can download from a known stable mirror or archive.org.
# For now, using a placeholder/reliable small SF2 link from a GitHub raw URL, 
# you may replace this URL with the exact preferred <20MB SF2 URL e.g. from FreePats.

$downloadUrl = "https://raw.githubusercontent.com/freepats/freepats-piano/master/Piano.sf2" # Placeholder URL

Write-Host "Downloading default piano soundfont..."
Write-Host "From: $downloadUrl"
Write-Host "To:   $targetFile"

try {
    Invoke-WebRequest -Uri $downloadUrl -OutFile $targetFile
    Write-Host "Download complete!"
} catch {
    Write-Host "Error downloading asset: $_" -ForegroundColor Red
    Write-Host "Please download a small SF2 file manually and place it at: $targetFile"
    exit 1
}
