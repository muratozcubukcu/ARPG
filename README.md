# C++ RPG Game Engine

A comprehensive 2D RPG game engine built in C++ with modular architecture and extensible design patterns.

## Features

- **Modular Architecture**: Clean separation of concerns with dedicated systems for different game aspects
- **Entity Component System**: Flexible character, mob, and object management
- **Combat System**: Abilities, status effects, and stat-based combat mechanics
- **Dungeon Generation**: Procedural dungeon creation and management
- **Physics System**: Collision detection and physics simulation
- **Input Management**: Responsive input handling for player controls
- **Camera System**: Dynamic camera following and viewport management

## Project Structure

```
rpg_cpp/
├── core/           # Core game engine systems
│   ├── gameengine.cpp/h
│   ├── input_manager.cpp/h
│   └── physics_system.cpp/h
├── entities/       # Game entities and components
│   ├── character.cpp/h
│   ├── mob.cpp/h
│   ├── ability.cpp/h
│   └── statusEffect.cpp/h
├── systems/        # Game systems
│   ├── class.cpp/h
│   ├── race.cpp/h
│   ├── statblock.cpp/h
│   └── pointwell.h
├── world/          # World and environment
│   ├── dungeon.cpp/h
│   ├── position.cpp/h
│   └── camera.cpp/h
├── controllers/    # Player and AI controllers
│   └── player_controller.cpp/h
├── types.h         # Common type definitions
├── main.cpp        # Entry point
└── Makefile        # Build configuration
```

## Building the Project

### Prerequisites
- C++ compiler (GCC, Clang, or MSVC)
- Make (for Unix-like systems) or use the provided build.bat for Windows

### Build Commands

**Windows:**
```bash
build.bat
```

**Unix-like systems:**
```bash
make
```

**Manual compilation:**
```bash
g++ -std=c++17 -o rpg_game.exe *.cpp
```

## Running the Game

After successful compilation, run the executable:
```bash
./rpg_game.exe
```

## Architecture Highlights

- **Scalable Design**: Uses design patterns that avoid excessive if/else statements for class-specific logic
- **Component-Based**: Entities are composed of multiple components for flexibility
- **Event-Driven**: Systems communicate through events rather than tight coupling
- **Extensible**: Easy to add new character classes, abilities, and game mechanics

## Development

This project emphasizes clean, maintainable code with:
- Clear separation of concerns
- Minimal dependencies between systems
- Consistent naming conventions
- Comprehensive error handling

## License

This project is open source and available under the MIT License.
