@echo off
echo Building RPG C++ Project...

REM Set compiler and flags
set CXX=g++
set CXXFLAGS=-std=c++17 -Wall -Wextra -g

REM Source files
set SOURCES=main.cpp ability.cpp character.cpp class.cpp race.cpp mob.cpp statblock.cpp statuseffect.cpp gameengine.cpp player_controller.cpp camera.cpp input_manager.cpp physics_system.cpp position.cpp

REM Test source files
set TEST_SOURCES=test_statuseffects.cpp ability.cpp character.cpp class.cpp race.cpp mob.cpp statblock.cpp statuseffect.cpp gameengine.cpp player_controller.cpp camera.cpp input_manager.cpp physics_system.cpp position.cpp

REM Live movement test source files
set LIVE_MOVEMENT_SOURCES=test_livemovement.cpp ability.cpp character.cpp class.cpp race.cpp mob.cpp statblock.cpp statuseffect.cpp gameengine.cpp player_controller.cpp camera.cpp input_manager.cpp physics_system.cpp position.cpp

REM Clean previous build
echo Cleaning previous build...
del /Q *.o 2>nul
del /Q rpg_game.exe 2>nul
del /Q test_statuseffects.exe 2>nul
del /Q test_livemovement.exe 2>nul

REM Build main game
echo Building main game...
%CXX% %CXXFLAGS% -c %SOURCES%
%CXX% *.o -o rpg_game.exe

REM Build live movement test
echo Building live movement test...
del /Q *.o 2>nul
%CXX% %CXXFLAGS% -c %LIVE_MOVEMENT_SOURCES%
%CXX% *.o -o test_livemovement.exe

REM Build test
echo Building test executable...
del /Q *.o 2>nul
%CXX% %CXXFLAGS% -c %TEST_SOURCES%
%CXX% *.o -o test_statuseffects.exe

REM Clean up object files
del /Q *.o 2>nul

echo Build complete!
echo.
echo Available executables:
echo - rpg_game.exe (main game)
echo - test_livemovement.exe (live WASD movement test)
echo - test_statuseffects.exe (status effect test)
echo.
echo To test live movement: test_livemovement.exe
echo To run main game: rpg_game.exe
