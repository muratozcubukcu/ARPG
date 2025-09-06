# PS1-Style 3D Graphics System

This document explains how to use the PS1-style 3D graphics system implemented for your RPG game.

## Overview

The PS1 graphics system provides authentic PlayStation 1-style 3D rendering with:
- Low-poly models with quantized vertices
- Vertex-based lighting with quantization
- Affine texture mapping (no perspective correction)
- Vertex-based fog
- Flat shading support
- Dithering and color quantization
- First-person camera controls

## Components

### 1. PS1StyleRenderer
The main renderer that extends the Vulkan forward renderer with PS1-specific features.

```cpp
#include "ps1_renderer.h"

// Create renderer
auto renderer = std::make_unique<VKFW::Systems::PS1StyleRenderer>();

// Configure PS1 settings
VKFW::PS1Settings settings;
settings.affineTextureMapping = true;
settings.vertexLighting = true;
settings.quantizedLighting = true;
settings.lightingLevels = 4;
settings.vertexFog = true;
settings.fogDensity = 1.0f;
settings.useVertexColors = true;
settings.flatShading = false;
settings.fov = 60.0f;
settings.nearPlane = 0.1f;
settings.farPlane = 100.0f;
settings.noPerspectiveCorrection = true;
settings.dithering = true;
settings.colorQuantization = true;
settings.colorBits = 16;

renderer->set_ps1_settings(settings);
```

### 2. PS1Camera
First-person camera with PS1-style projection and fog support.

```cpp
#include "ps1_camera.h"

// Create camera
auto camera = std::make_shared<VKFW::Core::PS1Camera>("PS1Camera");
camera->setup_ps1_style();

// Update first-person view
camera->update_first_person(yaw, pitch, position);

// Setup fog
camera->set_fog_settings(0.5f, 1.0f, 1.0f, Vec3(0.5f, 0.5f, 0.5f));
```

### 3. PS1Mesh
Low-poly mesh system with PS1-style geometry processing.

```cpp
#include "ps1_mesh.h"

// Create mesh
auto mesh = std::make_shared<VKFW::Core::PS1Mesh>("MyMesh");
mesh->setup_ps1_style();

// Configure mesh settings
VKFW::Core::PS1MeshSettings meshSettings;
meshSettings.affineTextureMapping = true;
meshSettings.vertexColors = true;
meshSettings.flatShading = false;
meshSettings.quantizedVertices = true;
meshSettings.vertexGridSize = 0.1f;
meshSettings.lowPolyStyle = true;
meshSettings.maxVerticesPerFace = 3;

mesh->set_ps1_settings(meshSettings);

// Apply PS1-style geometry processing
mesh->quantize_vertices(0.1f);
mesh->enable_flat_shading();
mesh->convert_to_low_poly();
```

### 4. PS1Material
Material system with PS1-style rendering properties.

```cpp
#include "ps1_material.h"

// Create material
auto material = std::make_shared<VKFW::Core::PS1Material>("MyMaterial");
material->setup_ps1_style();

// Configure material
material->set_color(Vec3(1.0f, 0.5f, 0.2f));
material->set_metallic(0.0f);
material->set_roughness(0.8f);

// Configure PS1 settings
VKFW::Core::PS1MaterialSettings matSettings;
matSettings.useTexture = true;
matSettings.useVertexColors = true;
matSettings.quantizedLighting = true;
matSettings.lightingLevels = 4;
matSettings.useFog = true;
matSettings.fogDensity = 1.0f;
matSettings.dithering = true;
matSettings.colorQuantization = true;
matSettings.colorBits = 16;
matSettings.flatShading = false;
matSettings.affineTextureMapping = true;

material->set_ps1_settings(matSettings);
```

### 5. PS1GraphicsManager
High-level manager that integrates all PS1 graphics components.

```cpp
#include "ps1_graphics_manager.h"

// Create graphics manager
auto graphicsManager = std::make_unique<VKFW::PS1GraphicsManager>();

// Initialize
if (!graphicsManager->initialize()) {
    std::cerr << "Failed to initialize PS1 graphics" << std::endl;
    return -1;
}

// Setup PS1 style
graphicsManager->setup_ps1_style();

// Add meshes to scene
graphicsManager->add_mesh(mesh);

// Setup fog and lighting
graphicsManager->set_fog_settings(0.5f, 1.0f, 1.0f, Vec3(0.5f, 0.5f, 0.5f));
graphicsManager->set_lighting_settings(4, true);

// Render loop
while (running) {
    graphicsManager->render_frame();
}
```

## Integration with Game Engine

The PS1 graphics system is integrated with your existing game engine:

```cpp
// In your GameEngine class
#include "ps1_graphics_manager.h"

class GameEngine {
private:
    std::unique_ptr<PS1GraphicsManager> graphicsManager;
    
public:
    void initialize() {
        // Initialize graphics
        graphicsManager = std::make_unique<PS1GraphicsManager>();
        if (graphicsManager->initialize()) {
            graphicsManager->set_game_engine(this);
        }
    }
    
    void update(float deltaTime) {
        // Update graphics
        if (graphicsManager) {
            graphicsManager->update_from_game_state();
            graphicsManager->render_frame();
        }
    }
    
    void shutdown() {
        if (graphicsManager) {
            graphicsManager->shutdown();
        }
    }
};
```

## Shaders

The system includes PS1-style shaders:

- `ps1_vertex.glsl` - Vertex shader with PS1-style transformations
- `ps1_fragment.glsl` - Fragment shader with quantized lighting and fog

## Building

Add the PS1 graphics files to your build script:

```batch
set SOURCES=main.cpp ... ps1_renderer.cpp ps1_camera.cpp ps1_mesh.cpp ps1_material.cpp ps1_graphics_manager.cpp
```

## Usage Example

See `ps1_graphics_example.cpp` for a complete example of how to use the PS1 graphics system.

## PS1-Style Features

### Visual Characteristics
- **Low-poly models**: Simplified geometry with quantized vertices
- **Vertex lighting**: Lighting calculated per vertex, not per pixel
- **Quantized lighting**: Limited lighting levels (typically 4-8 levels)
- **Affine texture mapping**: No perspective correction for textures
- **Vertex fog**: Fog calculated per vertex
- **Flat shading**: Option for flat-shaded surfaces
- **Dithering**: Ordered dithering for color depth simulation
- **Color quantization**: Reduced color depth (16-bit)

### Performance
- Optimized for PS1-era hardware constraints
- Vertex-based calculations instead of per-pixel
- Simplified lighting models
- Reduced texture precision

## Customization

You can customize the PS1 style by adjusting the various settings:

- `lightingLevels`: Number of lighting quantization levels (2-8)
- `vertexGridSize`: Grid size for vertex quantization
- `fogDensity`: Fog effect intensity
- `colorBits`: Color depth simulation (8, 16, 24)
- `flatShading`: Enable/disable flat shading
- `dithering`: Enable/disable dithering effects

This system provides an authentic PS1-era 3D graphics experience while leveraging modern Vulkan rendering capabilities.
