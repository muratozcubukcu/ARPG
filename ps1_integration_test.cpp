#include "gameengine.h"
#include "ps1_graphics_manager.h"
#include "ps1_mesh.h"
#include "ps1_material.h"
#include "character.h"
#include "mob.h"
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>

// Test program that integrates PS1 graphics with the existing game engine
int main() {
    std::cout << "=== PS1 Graphics Integration Test ===" << std::endl;
    
    // Create game engine
    GameEngine gameEngine(60.0f, false);
    
    // Initialize game engine (this will also initialize PS1 graphics)
    gameEngine.initialize();
    
    // Get graphics manager
    auto& graphics = gameEngine.getGraphicsManager();
    
    // Setup PS1-style rendering
    graphics.setup_ps1_style();
    
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
    
    graphics.set_ps1_settings(settings);
    
    // Setup fog and lighting
    graphics.set_fog_settings(0.3f, 0.8f, 1.2f, VKFW::Vec3(0.6f, 0.7f, 0.8f));
    graphics.set_lighting_settings(4, true);
    graphics.enable_ps1_effects(true);
    
    // Create some test characters
    std::cout << "Creating test characters..." << std::endl;
    
    // Create a player character
    Character player("TestPlayer", "Human", "Warrior", 1);
    player.setPosition(0.0, 0.0, 0.0);
    gameEngine.addCharacter(player);
    
    // Create some test mobs
    Mob goblin("Goblin", "Goblin", "Warrior", 1);
    goblin.setPosition(5.0, 0.0, 5.0);
    gameEngine.addMob(goblin);
    
    Mob orc("Orc", "Orc", "Warrior", 2);
    orc.setPosition(-5.0, 0.0, 5.0);
    gameEngine.addMob(orc);
    
    // Create PS1 meshes for the characters
    auto playerMesh = std::make_shared<VKFW::Core::PS1Mesh>("PlayerMesh");
    playerMesh->setup_ps1_style();
    
    VKFW::Core::PS1MeshSettings meshSettings;
    meshSettings.affineTextureMapping = true;
    meshSettings.vertexColors = true;
    meshSettings.flatShading = false;
    meshSettings.quantizedVertices = true;
    meshSettings.vertexGridSize = 0.1f;
    meshSettings.lowPolyStyle = true;
    meshSettings.maxVerticesPerFace = 3;
    
    playerMesh->set_ps1_settings(meshSettings);
    playerMesh->quantize_vertices(0.1f);
    playerMesh->enable_flat_shading();
    
    graphics.add_mesh(playerMesh);
    
    // Create PS1 materials
    auto playerMaterial = std::make_shared<VKFW::Core::PS1Material>("PlayerMaterial");
    playerMaterial->setup_ps1_style();
    playerMaterial->set_color(VKFW::Vec3(0.8f, 0.6f, 0.4f)); // Skin color
    
    // Get camera for first-person controls
    auto camera = graphics.get_camera();
    if (camera) {
        camera->setup_ps1_style();
        camera->update_first_person(0.0f, 0.0f, VKFW::Vec3(0.0f, 2.0f, 5.0f));
    }
    
    // Run the game loop
    std::cout << "Starting integrated game loop..." << std::endl;
    std::cout << "This will run the game engine with PS1 graphics for 10 seconds..." << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto lastTime = startTime;
    int frameCount = 0;
    
    while (frameCount < 600) { // Run for 10 seconds at 60 FPS
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        // Update game engine (this includes graphics rendering)
        gameEngine.update(deltaTime);
        
        // Simulate camera movement
        if (camera) {
            float time = frameCount * 0.016f;
            float yaw = time * 30.0f; // Slow rotation
            float pitch = std::sin(time * 0.2f) * 10.0f; // Gentle pitch movement
            VKFW::Vec3 position(0.0f, 2.0f + std::sin(time * 0.3f) * 1.0f, 5.0f);
            camera->update_first_person(yaw, pitch, position);
        }
        
        // Print status every 60 frames
        if (frameCount % 60 == 0) {
            std::cout << "Frame " << frameCount << " - Game running with PS1 graphics" << std::endl;
        }
        
        frameCount++;
        
        // Frame rate limiting
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
    
    std::cout << "PS1 Graphics Integration Test completed" << std::endl;
    std::cout << "Total frames rendered: " << frameCount << std::endl;
    
    // Shutdown game engine (this will also shutdown graphics)
    gameEngine.shutdown();
    
    return 0;
}
