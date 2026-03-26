@echo off
setlocal enabledelayedexpansion

:: Karakter kodlamasini UTF-8 yap (Turkce karakterler icin)
chcp 65001 >nul

echo ===================================================
echo [BILGI] Sistem kontrol ediliyor...

:: Klasorler
if not exist input mkdir input
if not exist output mkdir output

:: 1. g++ Denemesi
echo [BILGI] g++ deneniyor...
where g++ >nul 2>&1
if %errorlevel% equ 0 (
    g++ src/main.cpp -o app.exe -O3 -Wall -I external -static -static-libgcc -static-libstdc++
    if exist app.exe goto :SUCCESS
)

:: 2. MSVC Denemesi (Sisteminizdeki Ozel Yol - VS 18)
echo [BILGI] Visual Studio (v18) deneniyor...
set "V18=C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
if exist "%V18%" (
    call "%V18%" >nul
    cl.exe /O2 /MT /EHsc /I external src\main.cpp /Fe:app.exe
    if exist app.exe goto :SUCCESS
)

:: 3. MSVC Denemesi (Genel VS 2022)
echo [BILGI] Visual Studio 2019 deneniyor...
set "V19=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
if exist "%V19%" (
    call "%V19%" >nul
    cl.exe /O2 /MT /EHsc /I external src\main.cpp /Fe:app.exe
    if exist app.exe goto :SUCCESS
)

:RUN_ONLY
if exist app.exe (
    echo [BILGI] Mevcut app.exe calistiriliyor...
    goto :SUCCESS
)

echo [HATA] Derleyici bulunamadi ve app.exe mevcut degil!
echo Lutfen MinGW (g++) veya Visual Studio kurulu oldugundan emin olun.
pause
exit /b 1

:SUCCESS
echo [TAMAM] Program baslatiliyor...
echo ---------------------------------------------------
app.exe
echo ---------------------------------------------------
echo [BILGI] Program sonlandi.
pause
