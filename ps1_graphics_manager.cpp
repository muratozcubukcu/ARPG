#include "ps1_graphics_manager.h"
#include "gameengine.h"
#include "player_controller.h"
#include <iostream>

namespace VKFW {

PS1GraphicsManager::PS1GraphicsManager() : m_gameEngine(nullptr) {
    setup_ps1_style();
}

bool PS1GraphicsManager::initialize() {
    try {
        // Initialize PS1 renderer
        m_renderer = std::make_unique<Systems::PS1StyleRenderer>();
        if (!m_renderer) {
            std::cerr << "Failed to create PS1 renderer" << std::endl;
            return false;
        }
        
        // Setup PS1 camera
        setup_ps1_camera();
        
        // Setup PS1 materials
        setup_ps1_materials();
        
        std::cout << "PS1 Graphics Manager initialized successfully" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize PS1 Graphics Manager: " << e.what() << std::endl;
        return false;
    }
}

void PS1GraphicsManager::shutdown() {
    clear_scene();
    m_renderer.reset();
    m_camera.reset();
    m_materials.clear();
}

void PS1GraphicsManager::setup_ps1_style() {
    // Configure PS1-style settings
    m_ps1Settings.affineTextureMapping = true;
    m_ps1Settings.vertexLighting = true;
    m_ps1Settings.quantizedLighting = true;
    m_ps1Settings.lightingLevels = 4;
    m_ps1Settings.vertexFog = true;
    m_ps1Settings.fogDensity = 1.0f;
    m_ps1Settings.useVertexColors = true;
    m_ps1Settings.flatShading = false;
    m_ps1Settings.fov = 60.0f;
    m_ps1Settings.nearPlane = 0.1f;
    m_ps1Settings.farPlane = 100.0f;
    m_ps1Settings.noPerspectiveCorrection = true;
    m_ps1Settings.dithering = true;
    m_ps1Settings.colorQuantization = true;
    m_ps1Settings.colorBits = 16;
}

void PS1GraphicsManager::set_ps1_settings(const PS1Settings& settings) {
    m_ps1Settings = settings;
    if (m_renderer) {
        m_renderer->set_ps1_settings(settings);
    }
}

void PS1GraphicsManager::update_camera(float deltaTime) {
    if (!m_camera) return;
    
    // Update camera from game engine if available
    if (m_gameEngine) {
        // Get player position and rotation from game engine
        // This would integrate with your existing player controller
        Vec3 playerPos = Vec3(0.0f, 0.0f, 0.0f); // Get from player controller
        float yaw = 0.0f; // Get from player controller
        float pitch = 0.0f; // Get from player controller
        
        m_camera->update_first_person(yaw, pitch, playerPos);
    }
    
    // Process input
    process_input(deltaTime);
}

void PS1GraphicsManager::process_input(float deltaTime) {
    process_mouse_input(deltaTime);
    process_keyboard_input(deltaTime);
}

void PS1GraphicsManager::add_mesh(std::shared_ptr<Core::PS1Mesh> mesh) {
    if (mesh) {
        m_meshes.push_back(mesh);
    }
}

void PS1GraphicsManager::remove_mesh(std::shared_ptr<Core::PS1Mesh> mesh) {
    auto it = std::find(m_meshes.begin(), m_meshes.end(), mesh);
    if (it != m_meshes.end()) {
        m_meshes.erase(it);
    }
}

void PS1GraphicsManager::clear_scene() {
    m_meshes.clear();
}

void PS1GraphicsManager::render_frame() {
    if (!m_renderer || !m_camera) return;
    
    // Update camera
    update_camera(0.016f); // Assume 60 FPS
    
    // Render PS1 scene
    render_ps1_scene();
}

void PS1GraphicsManager::render_ps1_scene() {
    if (!m_renderer || !m_camera) return;
    
    // Convert PS1 meshes to regular meshes for rendering
    std::vector<Core::Mesh*> renderMeshes;
    for (auto& mesh : m_meshes) {
        renderMeshes.push_back(mesh.get());
    }
    
    // Render with PS1 renderer
    m_renderer->render_ps1_scene(nullptr, m_camera.get());
}

void PS1GraphicsManager::update_from_game_state() {
    if (!m_gameEngine) return;
    
    // Update graphics based on game state
    // This would integrate with your existing game systems
}

void PS1GraphicsManager::enable_ps1_effects(bool enable) {
    m_ps1Settings.dithering = enable;
    m_ps1Settings.colorQuantization = enable;
    m_ps1Settings.quantizedLighting = enable;
    
    if (m_renderer) {
        m_renderer->set_ps1_settings(m_ps1Settings);
    }
}

void PS1GraphicsManager::set_fog_settings(float start, float end, float density, const Vec3& color) {
    if (m_camera) {
        m_camera->set_fog_settings(start, end, density, color);
    }
}

void PS1GraphicsManager::set_lighting_settings(int levels, bool quantized) {
    m_ps1Settings.lightingLevels = levels;
    m_ps1Settings.quantizedLighting = quantized;
    
    if (m_renderer) {
        m_renderer->set_ps1_settings(m_ps1Settings);
    }
}

void PS1GraphicsManager::setup_ps1_camera() {
    m_camera = std::make_shared<Core::PS1Camera>("PS1Camera");
    m_camera->setup_ps1_style();
}

void PS1GraphicsManager::setup_ps1_materials() {
    // Create default PS1 materials
    auto defaultMaterial = std::make_shared<Core::PS1Material>("DefaultPS1Material");
    defaultMaterial->setup_ps1_style();
    m_materials.push_back(defaultMaterial);
}

void PS1GraphicsManager::update_ps1_lighting() {
    // Update PS1-specific lighting
    // This would integrate with your existing lighting system
}

void PS1GraphicsManager::apply_ps1_post_processing() {
    // Apply PS1-specific post-processing effects
    // This would integrate with your existing post-processing system
}

void PS1GraphicsManager::process_mouse_input(float deltaTime) {
    // Process mouse input for camera control
    // This would integrate with your existing input system
}

void PS1GraphicsManager::process_keyboard_input(float deltaTime) {
    // Process keyboard input for camera movement
    // This would integrate with your existing input system
}

} // namespace VKFW
