# RPG C++ Project Makefile
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
TARGET = rpg_game
TEST_TARGET = test_statuseffects
LIVE_MOVEMENT_TARGET = test_livemovement
MISSING_TEST_TARGET = test_missing_statuseffects
STATUS_EFFECTS_TEST_TARGET = test_status_effects

# Source files
SOURCES = main.cpp \
          ability.cpp \
          character.cpp \
          class.cpp \
          race.cpp \
          mob.cpp \
          statblock.cpp \
          statuseffect.cpp \
          gameengine.cpp \
          position.cpp \
          player_controller.cpp \
          camera.cpp \
          input_manager.cpp \
          physics_system.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Test source files
TEST_SOURCES = test_statuseffects.cpp \
               ability.cpp \
               character.cpp \
               class.cpp \
               race.cpp \
               mob.cpp \
               statblock.cpp \
               statuseffect.cpp \
               gameengine.cpp \
               player_controller.cpp \
               camera.cpp \
               input_manager.cpp \
               physics_system.cpp \
               position.cpp

# Live movement test source files
LIVE_MOVEMENT_SOURCES = test_livemovement.cpp \
                        ability.cpp \
                        character.cpp \
                        class.cpp \
                        race.cpp \
                        mob.cpp \
                        statblock.cpp \
                        statuseffect.cpp \
                        gameengine.cpp \
                        player_controller.cpp \
                        camera.cpp \
                        input_manager.cpp \
                        physics_system.cpp \
                        position.cpp

# Missing StatusEffect test source files
MISSING_TEST_SOURCES = test_missing_statuseffects.cpp \
                       ability.cpp \
                       character.cpp \
                       class.cpp \
                       race.cpp \
                       mob.cpp \
                       statblock.cpp \
                       statuseffect.cpp \
                       gameengine.cpp \
                       player_controller.cpp \
                       camera.cpp \
                       input_manager.cpp \
                       physics_system.cpp \
                       position.cpp

# Status Effects test source files
STATUS_EFFECTS_TEST_SOURCES = test_status_effects.cpp \
                             ability.cpp \
                             character.cpp \
                             class.cpp \
                             race.cpp \
                             mob.cpp \
                             statblock.cpp \
                             statuseffect.cpp \
                             gameengine.cpp \
                             player_controller.cpp \
                             camera.cpp \
                             input_manager.cpp \
                             physics_system.cpp \
                             position.cpp

# Test object files
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)
LIVE_MOVEMENT_OBJECTS = $(LIVE_MOVEMENT_SOURCES:.cpp=.o)
MISSING_TEST_OBJECTS = $(MISSING_TEST_SOURCES:.cpp=.o)
STATUS_EFFECTS_TEST_OBJECTS = $(STATUS_EFFECTS_TEST_SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Main game executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

# Live movement test executable
$(LIVE_MOVEMENT_TARGET): $(LIVE_MOVEMENT_OBJECTS)
	$(CXX) $(LIVE_MOVEMENT_OBJECTS) -o $(LIVE_MOVEMENT_TARGET)

# Test executable
$(TEST_TARGET): $(TEST_OBJECTS)
	$(CXX) $(TEST_OBJECTS) -o $(TEST_TARGET)

# Missing StatusEffect test executable
$(MISSING_TEST_TARGET): $(MISSING_TEST_OBJECTS)
	$(CXX) $(MISSING_TEST_OBJECTS) -o $(MISSING_TEST_TARGET)

# Status Effects test executable
$(STATUS_EFFECTS_TEST_TARGET): $(STATUS_EFFECTS_TEST_OBJECTS)
	$(CXX) $(STATUS_EFFECTS_TEST_OBJECTS) -o $(STATUS_EFFECTS_TEST_TARGET)

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	del /Q *.o $(TARGET).exe $(TEST_TARGET).exe $(LIVE_MOVEMENT_TARGET).exe $(MISSING_TEST_TARGET).exe $(STATUS_EFFECTS_TEST_TARGET).exe 2>nul || true

# Clean and rebuild
rebuild: clean all

# Run the main game
run: $(TARGET)
	./$(TARGET)

# Run the live movement test
test_movement: $(LIVE_MOVEMENT_TARGET)
	./$(LIVE_MOVEMENT_TARGET)

# Run the test
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Run the missing StatusEffect test
test_missing: $(MISSING_TEST_TARGET)
	./$(MISSING_TEST_TARGET)

# Run the status effects test
test_status: $(STATUS_EFFECTS_TEST_TARGET)
	./$(STATUS_EFFECTS_TEST_TARGET)

# Phony targets
.PHONY: all clean rebuild run test test_missing test_movement test_status

# Dependencies
ability.o: ability.h types.h character.h mob.h
character.o: character.h race.h class.h statblock.h position.h statuseffect.h
class.o: class.h types.h ability.h
race.o: race.h types.h
mob.o: mob.h types.h race.h statblock.h position.h statuseffect.h
statblock.o: statblock.h types.h
statuseffect.o: statuseffect.h types.h character.h mob.h
gameengine.o: gameengine.h types.h character.h mob.h ability.h movementsystem.h
movementsystem.o: movementsystem.h types.h character.h mob.h gameengine.h
inputhandler.o: inputhandler.h movementsystem.h
position.o: position.h
main.o: gameengine.h character.h class.h race.h
test_statuseffects.o: statuseffect.h character.h class.h race.h mob.h gameengine.h
test_livemovement.o: gameengine.h character.h class.h race.h movementsystem.h inputhandler.h
test_missing_statuseffects.o: statuseffect.h character.h class.h race.h mob.h gameengine.h
test_status_effects.o: statuseffect.h character.h class.h race.h mob.h gameengine.h
