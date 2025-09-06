@echo off
echo Building RPG C++ Project...

REM Set compiler and flags
set CXX=g++
set CXXFLAGS=-std=c++17 -Wall -Wextra -g -ISFML\include -Iglm -IC:\MinGW\include
set TARGET=rpg_game.exe

REM Source files
set SOURCES=main.cpp ability.cpp character.cpp class.cpp race.cpp mob.cpp statblock.cpp statuseffect.cpp gameengine.cpp player_controller.cpp camera.cpp input_manager.cpp physics_system.cpp position.cpp inventory.cpp item.cpp loot_system.cpp dungeon.cpp

REM Parse command line arguments
if "%1"=="run" (
    echo Building and running main game...
    %CXX% %CXXFLAGS% -o %TARGET% %SOURCES% -LSFML\lib -lsfml-graphics -lsfml-window -lsfml-system -lopengl32 -lgdi32
    if %ERRORLEVEL% EQU 0 (
        echo Running %TARGET%...
        %TARGET%
    )
) else if "%1"=="test" (
    echo Building and running tests...
    %CXX% %CXXFLAGS% -o test_dungeon_system.exe test_dungeon_system.cpp %SOURCES%
    if %ERRORLEVEL% EQU 0 (
        echo Running dungeon system test...
        test_dungeon_system.exe
    )
    %CXX% %CXXFLAGS% -o test_inventory.exe test_inventory.cpp %SOURCES%
    if %ERRORLEVEL% EQU 0 (
        echo Running inventory test...
        test_inventory.exe
    )
    %CXX% %CXXFLAGS% -o test_loot_system.exe test_loot_system.cpp %SOURCES%
    if %ERRORLEVEL% EQU 0 (
        echo Running loot system test...
        test_loot_system.exe
    )
) else if "%1"=="ps1" (
    echo Building and running simple PS1 graphics test...
    %CXX% %CXXFLAGS% -o simple_ps1_test.exe simple_ps1_basic_test.cpp simple_ps1_basic.cpp -LSFML\lib -lsfml-graphics -lsfml-window -lsfml-system -lopengl32 -lgdi32 -lwinmm -luser32 -lkernel32 -ladvapi32
    if %ERRORLEVEL% EQU 0 (
        echo Running simple PS1 graphics test...
        simple_ps1_test.exe
    )
) else if "%1"=="clean" (
    echo Cleaning build files...
    del /Q *.o *.exe 2>nul
    echo Clean complete!
) else if "%1"=="" (
    echo.
    echo Usage: build.bat [run^|test^|ps1^|clean]
    echo   run   - Build and run the main game
    echo   test  - Build and run all tests
    echo   ps1   - Build and run simple PS1 graphics test
    echo   clean - Clean build files
    echo.
    echo Building main executable...
    %CXX% %CXXFLAGS% -o %TARGET% %SOURCES% -LSFML\lib -lsfml-graphics -lsfml-window -lsfml-system -lopengl32 -lgdi32
    if %ERRORLEVEL% EQU 0 (
        echo Build successful! Created %TARGET%
    ) else (
        echo Build failed with error code %ERRORLEVEL%
        exit /b %ERRORLEVEL%
    )
) else (
    echo Unknown command: %1
    echo Usage: build.bat [run^|test^|ps1^|clean]
    exit /b 1
)