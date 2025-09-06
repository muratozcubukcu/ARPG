#include "ps1_graphics_manager.h"
#include "ps1_mesh.h"
#include "ps1_material.h"
#include "ps1_camera.h"
#include <iostream>
#include <memory>

// Example of how to use the PS1 graphics system
int main() {
    std::cout << "PS1 Graphics System Example" << std::endl;
    
    // Create graphics manager
    auto graphicsManager = std::make_unique<VKFW::PS1GraphicsManager>();
    
    // Initialize the graphics system
    if (!graphicsManager->initialize()) {
        std::cerr << "Failed to initialize PS1 graphics system" << std::endl;
        return -1;
    }
    
    // Setup PS1-style rendering
    graphicsManager->setup_ps1_style();
    
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
    
    graphicsManager->set_ps1_settings(settings);
    
    // Create a PS1-style mesh
    auto mesh = std::make_shared<VKFW::Core::PS1Mesh>("ExampleMesh");
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
    
    // Add mesh to scene
    graphicsManager->add_mesh(mesh);
    
    // Setup fog
    graphicsManager->set_fog_settings(0.5f, 1.0f, 1.0f, VKFW::Vec3(0.5f, 0.5f, 0.5f));
    
    // Setup lighting
    graphicsManager->set_lighting_settings(4, true);
    
    // Enable PS1 effects
    graphicsManager->enable_ps1_effects(true);
    
    // Get camera for first-person controls
    auto camera = graphicsManager->get_camera();
    if (camera) {
        // Setup first-person camera
        camera->setup_ps1_style();
        
        // Set initial position and rotation
        camera->update_first_person(0.0f, 0.0f, VKFW::Vec3(0.0f, 0.0f, 5.0f));
    }
    
    // Render loop (simplified)
    std::cout << "Starting PS1-style rendering loop..." << std::endl;
    for (int frame = 0; frame < 100; ++frame) {
        // Update camera (simulate mouse movement)
        float yaw = frame * 0.1f;
        float pitch = std::sin(frame * 0.05f) * 10.0f;
        camera->update_first_person(yaw, pitch, VKFW::Vec3(0.0f, 0.0f, 5.0f));
        
        // Render frame
        graphicsManager->render_frame();
        
        // Simulate frame time
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
    
    std::cout << "PS1 Graphics System Example completed" << std::endl;
    
    // Cleanup
    graphicsManager->shutdown();
    
    return 0;
}
