@echo off
echo Building STM32F429 TouchGFX Touchpad Project...
echo.

cd /d "%~dp0gcc"

echo Cleaning previous build...
make clean

echo.
echo Building project...
make all

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo BUILD SUCCESS!
    echo ========================================
    echo.
    echo Output files:
    echo - STM32F429I_DISCO_REV_D01.elf
    echo - STM32F429I_DISCO_REV_D01.hex
    echo - STM32F429I_DISCO_REV_D01.bin
    echo.
    echo Ready to flash to STM32F429 Discovery Board!
) else (
    echo.
    echo ========================================
    echo BUILD FAILED!
    echo ========================================
    echo.
    echo Please check the error messages above.
)

pause
