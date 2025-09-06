# PS1 Graphics Test Programs

This document explains how to test the PS1-style 3D graphics system with movement and look-around controls using your assets.

## Test Programs

### 1. `ps1_test.cpp` - Basic PS1 Graphics Test
A simple test that loads your converted OBJ models and demonstrates PS1-style rendering with basic movement.

**Features:**
- Loads models from `assets/converted/Forrest/`
- Circular movement pattern
- PS1-style rendering effects
- Basic camera controls

**Usage:**
```batch
build.bat ps1
```

### 2. `ps1_advanced_test.cpp` - Advanced PS1 Graphics Test
An enhanced test with more complex movement patterns and HDR environment loading.

**Features:**
- Loads all available OBJ models
- Loads HDR environments from `assets/Sky/`
- Complex movement patterns
- Enhanced input simulation
- Better vertex data handling

**Usage:**
```batch
build.bat ps1
```

### 3. `ps1_integration_test.cpp` - Game Engine Integration Test
Tests the PS1 graphics system integrated with your existing game engine.

**Features:**
- Uses your existing GameEngine class
- Creates test characters and mobs
- Integrates with physics system
- Shows how PS1 graphics work with RPG mechanics

**Usage:**
```batch
build.bat ps1
```

## Assets Used

The test programs will attempt to load the following assets:

### 3D Models (OBJ format)
- `assets/converted/Forrest/trees/Tree1.obj`
- `assets/converted/Forrest/trees/Tree2.obj`
- `assets/converted/Forrest/rocks/Rock1.obj`

### HDR Environments
- `assets/Sky/Daysky_HDRI/DAYSKY.hdr`
- `assets/Sky/PSX_Nightsky_HDRI/PSX_NIGHTSKY.hdr`

## Running the Tests

### Quick Test
```batch
build.bat ps1
```

This will build and run all three PS1 graphics tests.

### Individual Tests
```batch
# Build all
build.bat

# Run specific test
ps1_test.exe
ps1_advanced_test.exe
ps1_integration_test.exe
```

## Expected Behavior

### Visual Effects
- **Low-poly models**: Quantized vertices for PS1 look
- **Vertex lighting**: 4-level quantized lighting
- **Vertex fog**: Atmospheric fog effect
- **Flat shading**: Option for flat-shaded surfaces
- **Dithering**: Ordered dithering for color depth
- **Color quantization**: 16-bit color simulation

### Movement Patterns
- **Circular movement**: Camera moves in a circle around the scene
- **Height variation**: Gentle vertical movement
- **Look around**: Smooth yaw and pitch rotation
- **Fog effects**: Distance-based fog for atmosphere

### Performance
- **60 FPS target**: Frame rate limiting for smooth animation
- **PS1-style constraints**: Authentic PlayStation 1 limitations
- **Efficient rendering**: Optimized for PS1-era hardware simulation

## Troubleshooting

### Common Issues

1. **Missing assets**: Ensure OBJ files exist in the correct paths
2. **Build errors**: Check that all PS1 graphics files are included in build
3. **Runtime errors**: Verify Vulkan drivers and graphics setup

### Debug Output

The tests include extensive debug output:
- Asset loading status
- Frame count and timing
- Camera position and rotation
- PS1 settings configuration

### Customization

You can modify the test programs to:
- Change movement patterns
- Adjust PS1 settings
- Load different assets
- Modify camera behavior
- Add more complex scenes

## Integration with Your Game

The `ps1_integration_test.cpp` shows how to integrate PS1 graphics with your existing game:

```cpp
// Get graphics manager from game engine
auto& graphics = gameEngine.getGraphicsManager();

// Setup PS1 style
graphics.setup_ps1_style();

// Add meshes to scene
graphics.add_mesh(mesh);

// The graphics system is automatically integrated with your game loop
```

## Next Steps

1. **Run the tests** to see PS1 graphics in action
2. **Modify the movement** to use your input system
3. **Load your own models** and textures
4. **Integrate with your RPG mechanics**
5. **Customize PS1 settings** for your desired look

The test programs provide a solid foundation for implementing PS1-style 3D graphics in your RPG game!
