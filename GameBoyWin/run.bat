@echo off
chcp 65001 > nul

:: 1. ПРИМУСОВО переходимо в папку, де лежить цей run.bat (GameBoyWin\GameBoyWin)
cd /d "%~dp0"

:: Одразу створюємо папку .temp на рівень вище (в корені), бо вона потрібна для компілятора та гри
if not exist ".temp" mkdir ".temp"

echo [Емулятор] Перевірка локального компілятора w64devkit у папці .temp...

:: Перевіряємо, чи існує компілятор за одним із можливих шляхів усередині папки .temp
if exist ".temp\w64devkit\w64devkit\bin\g++.exe" (
    set "COMPILER_PATH=%~dp0.temp\w64devkit\w64devkit\bin"
    goto :compiler_ready
)
if exist ".temp\w64devkit\bin\g++.exe" (
    set "COMPILER_PATH=%~dp0.temp\w64devkit\bin"
    goto :compiler_ready
)

:: Якщо компілятора в .temp немає, запускаємо автоматичну розпаковку ТУДИ
if exist "w64devkit-x64-2.8.0.7z.exe" (
    echo [Емулятор] Компілятор не знайдено в .temp. Початок автоматичної розпаковки...
    echo [Емулятор] Будь ласка, зачекайте, це може зайняти кілька секунд...
    
    :: Запуск SFX-архіву в тихому режимі з розпаковкою в папку .temp
    start /wait "" "w64devkit-x64-2.8.0.7z.exe" -y -o".temp"
    
    echo [Емулятор] Розпаковку в папку .temp завершено успішно!
    
    :: Повторно виставляємо шляхи після розпаковки
    if exist ".temp\w64devkit\w64devkit\bin\g++.exe" (
        set "COMPILER_PATH=%~dp0.temp\w64devkit\w64devkit\bin"
    ) else (
        set "COMPILER_PATH=%~dp0.temp\w64devkit\bin"
    )
) else (
    echo [ПОМИЛКА] Не знайдено ні компілятора в .temp, ні архіву 'w64devkit-x64-2.8.0.7z.exe' поруч.
    pause
    exit /b
)

:compiler_ready
:: Додаємо локальний g++ у змінні оточення (лише для цього вікна консолі)
set PATH=%COMPILER_PATH%;%PATH%

echo [Емулятор] Компіляція проекту з Raylib...

:: Однокрокова компіляція всього проекту одразу в папку .temp
set RAYLIB_DIR=raylib-6.0_win64_mingw-w64
g++ main.cpp -I./%RAYLIB_DIR%/include -I. -L./%RAYLIB_DIR%/lib -lraylib -lopengl32 -lgdi32 -lwinmm -o .temp\emulator.exe

:: 2. БЛОК ПЕРЕВІРКИ ТА ЗАПУСКУ ЕМУЛЯТОРА
if %ERRORLEVEL% EQU 0 (
    echo [Емулятор] Збірка успішна! Запуск програми...
    echo ---------------------------------------------------
    .temp\emulator.exe
) else (
    echo.
    echo [ПОМИЛКА] Не вдалося зібрати емулятор. Перевірте помилки компілятора вище.
    pause
)

cd ..