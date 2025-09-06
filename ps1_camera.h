#pragma once

#include <engine/core/scene/camera.h>
#include <memory>

namespace VKFW {
namespace Core {

struct PS1CameraSettings {
    float fov = 60.0f; // PS1 typical FOV
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    bool affineProjection = true; // No perspective correction
    bool vertexFog = true;
    float fogStart = 0.5f;
    float fogEnd = 1.0f;
    float fogDensity = 1.0f;
    Vec3 fogColor = Vec3(0.5f, 0.5f, 0.5f);
    
    // First-person controls
    float mouseSensitivity = 0.1f;
    float maxPitch = 89.0f;
    float minPitch = -89.0f;
    bool invertY = false;
};

class PS1Camera : public Camera {
public:
    PS1Camera();
    PS1Camera(const std::string& name);
    ~PS1Camera() = default;
    
    // PS1-specific setup
    void setup_ps1_style();
    void set_ps1_settings(const PS1CameraSettings& settings);
    const PS1CameraSettings& get_ps1_settings() const { return m_ps1Settings; }
    
    // First-person controls
    void update_first_person(float yaw, float pitch, const Vec3& position);
    void process_mouse_movement(float xOffset, float yOffset);
    void process_keyboard_input(int key, float deltaTime);
    
    // PS1-specific camera features
    void set_fog_settings(float start, float end, float density, const Vec3& color);
    void update_ps1_projection();
    void apply_ps1_style_rendering();
    
    // Override base camera methods
    void set_frustum() override;
    void update_view_matrix() override;
    
    // Getters for PS1-specific values
    float get_fog_start() const { return m_ps1Settings.fogStart; }
    float get_fog_end() const { return m_ps1Settings.fogEnd; }
    float get_fog_density() const { return m_ps1Settings.fogDensity; }
    Vec3 get_fog_color() const { return m_ps1Settings.fogColor; }
    
private:
    PS1CameraSettings m_ps1Settings;
    float m_currentYaw = 0.0f;
    float m_currentPitch = 0.0f;
    
    void clamp_pitch();
    void update_ps1_frustum();
    void calculate_ps1_projection_matrix();
};

} // namespace Core
} // namespace VKFW
