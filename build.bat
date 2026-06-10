@echo off
REM Force MSYS2 UCRT64 toolchain (avoids mingw64 / ucrt64 PATH conflicts).
set "MSYS2_UCRT=C:\msys64\ucrt64"
if not exist "%MSYS2_UCRT%\bin\g++.exe" (
    echo ERROR: UCRT64 g++ not found at %MSYS2_UCRT%\bin\g++.exe
    echo Install MSYS2 and run: pacman -S mingw-w64-ucrt-x86_64-gcc
    pause
    exit /b 1
)

set "PATH=%MSYS2_UCRT%\bin;%PATH%"

echo Compiling Kigali Smart Parking Management System (UCRT64)...
"%MSYS2_UCRT%\bin\g++.exe" -std=c++17 -I include ^
  src/main.cpp ^
  src/ParkingSystem.cpp ^
  src/SlotManager.cpp ^
  src/VehicleManager.cpp ^
  src/BillingManager.cpp ^
  src/ReportManager.cpp ^
  src/TariffManager.cpp ^
  src/Vehicle.cpp ^
  src/VehicleFactory.cpp ^
  src/TimeUtils.cpp ^
  src/InputValidator.cpp ^
  src/InputReader.cpp ^
  src/ConsoleStyle.cpp ^
  -o smart_parking.exe
if %ERRORLEVEL% EQU 0 (
    echo Build successful: smart_parking.exe
) else (
    echo Build failed. See errors above.
    echo Tip: Open "MSYS2 UCRT64" terminal or ensure PATH uses ucrt64, not mingw64.
)
pause
