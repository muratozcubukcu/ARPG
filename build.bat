@echo off
echo Building RPG C++ Project...

REM Set compiler and flags
set CXX=g++
set CXXFLAGS=-std=c++17 -Wall -Wextra -g

REM Source files
set SOURCES=main.cpp ability.cpp character.cpp class.cpp race.cpp mob.cpp statblock.cpp statuseffect.cpp gameengine.cpp player_controller.cpp camera.cpp input_manager.cpp physics_system.cpp position.cpp

REM Test source files
set TEST_SOURCES=test_statuseffects.cpp ability.cpp character.cpp class.cpp race.cpp mob.cpp statblock.cpp statuseffect.cpp gameengine.cpp player_controller.cpp camera.cpp input_manager.cpp physics_system.cpp position.cpp

REM Status effects test source files
set STATUS_EFFECTS_TEST_SOURCES=test_status_effects.cpp ability.cpp character.cpp class.cpp race.cpp mob.cpp statblock.cpp statuseffect.cpp gameengine.cpp player_controller.cpp camera.cpp input_manager.cpp physics_system.cpp position.cpp

REM Movement integration test source files
set MOVEMENT_INTEGRATION_TEST_SOURCES=test_movement_integration.cpp ability.cpp character.cpp class.cpp race.cpp mob.cpp statblock.cpp statuseffect.cpp gameengine.cpp player_controller.cpp camera.cpp input_manager.cpp physics_system.cpp position.cpp

REM Live movement test source files
set LIVE_MOVEMENT_SOURCES=test_livemovement.cpp ability.cpp character.cpp class.cpp race.cpp mob.cpp statblock.cpp statuseffect.cpp gameengine.cpp player_controller.cpp camera.cpp input_manager.cpp physics_system.cpp position.cpp

REM Clean previous build
echo Cleaning previous build...
del /Q *.o 2>nul
del /Q rpg_game.exe 2>nul
del /Q test_statuseffects.exe 2>nul
del /Q test_livemovement.exe 2>nul
del /Q test_status_effects.exe 2>nul
del /Q test_movement_integration.exe 2>nul

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

REM Build status effects test
echo Building status effects test executable...
del /Q *.o 2>nul
%CXX% %CXXFLAGS% -c %STATUS_EFFECTS_TEST_SOURCES%
%CXX% *.o -o test_status_effects.exe

REM Build movement integration test
echo Building movement integration test executable...
del /Q *.o 2>nul
%CXX% %CXXFLAGS% -c %MOVEMENT_INTEGRATION_TEST_SOURCES%
%CXX% *.o -o test_movement_integration.exe

REM Clean up object files
del /Q *.o 2>nul

echo Build complete!
echo.
echo Available executables:
echo - rpg_game.exe (main game)
echo - test_livemovement.exe (live WASD movement test)
echo - test_statuseffects.exe (status effect test)
echo - test_status_effects.exe (new status effects test)
echo - test_movement_integration.exe (movement integration test)
echo.
echo To test live movement: test_livemovement.exe
echo To run main game: rpg_game.exe
echo To test new status effects: test_status_effects.exe
echo To test movement integration: test_movement_integration.exe
