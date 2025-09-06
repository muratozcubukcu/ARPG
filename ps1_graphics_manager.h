#pragma once

#include "ps1_renderer.h"
#include "ps1_camera.h"
#include "ps1_mesh.h"
#include "ps1_material.h"
#include "gameengine.h"
#include <memory>
#include <vector>

namespace VKFW {

class PS1GraphicsManager {
public:
    PS1GraphicsManager();
    ~PS1GraphicsManager() = default;
    
    // Initialization
    bool initialize();
    void shutdown();
    
    // PS1 Graphics System
    void setup_ps1_style();
    void set_ps1_settings(const PS1Settings& settings);
    const PS1Settings& get_ps1_settings() const { return m_ps1Settings; }
    
    // Camera Management
    std::shared_ptr<Core::PS1Camera> get_camera() const { return m_camera; }
    void update_camera(float deltaTime);
    void process_input(float deltaTime);
    
    // Scene Management
    void add_mesh(std::shared_ptr<Core::PS1Mesh> mesh);
    void remove_mesh(std::shared_ptr<Core::PS1Mesh> mesh);
    void clear_scene();
    
    // Rendering
    void render_frame();
    void render_ps1_scene();
    
    // Integration with Game Engine
    void set_game_engine(GameEngine* engine) { m_gameEngine = engine; }
    void update_from_game_state();
    
    // PS1-specific features
    void enable_ps1_effects(bool enable);
    void set_fog_settings(float start, float end, float density, const Vec3& color);
    void set_lighting_settings(int levels, bool quantized);
    
private:
    // Core components
    std::unique_ptr<Systems::PS1StyleRenderer> m_renderer;
    std::shared_ptr<Core::PS1Camera> m_camera;
    std::vector<std::shared_ptr<Core::PS1Mesh>> m_meshes;
    std::vector<std::shared_ptr<Core::PS1Material>> m_materials;
    
    // Settings
    PS1Settings m_ps1Settings;
    
    // Game engine integration
    GameEngine* m_gameEngine;
    
    // Input handling
    float m_mouseSensitivity = 0.1f;
    bool m_mouseCaptured = false;
    
    // Internal methods
    void setup_ps1_camera();
    void setup_ps1_materials();
    void update_ps1_lighting();
    void apply_ps1_post_processing();
    
    // Input processing
    void process_mouse_input(float deltaTime);
    void process_keyboard_input(float deltaTime);
};

} // namespace VKFW
