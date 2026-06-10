# PowerShell build script — pins MSYS2 UCRT64 to avoid mingw/ucrt conflicts.
$UcrtBin = "C:\msys64\ucrt64\bin"
$Gpp = Join-Path $UcrtBin "g++.exe"

if (-not (Test-Path $Gpp)) {
    Write-Error "UCRT64 g++ not found at $Gpp. Install: pacman -S mingw-w64-ucrt-x86_64-gcc"
    exit 1
}

$env:PATH = "$UcrtBin;" + ($env:PATH -split ';' | Where-Object { $_ -notmatch 'msys64\\mingw64' }) -join ';'

$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $Root

Write-Host "Compiling with UCRT64 g++..."
& $Gpp -std=c++17 -I include `
  src/main.cpp `
  src/ParkingSystem.cpp `
  src/SlotManager.cpp `
  src/VehicleManager.cpp `
  src/BillingManager.cpp `
  src/ReportManager.cpp `
  src/TariffManager.cpp `
  src/Vehicle.cpp `
  src/VehicleFactory.cpp `
  src/TimeUtils.cpp `
  src/InputValidator.cpp `
  src/InputReader.cpp `
  src/ConsoleStyle.cpp `
  -o smart_parking.exe

if ($LASTEXITCODE -eq 0) {
    Write-Host "Build successful: smart_parking.exe" -ForegroundColor Green
} else {
    Write-Error "Build failed."
    exit 1
}
