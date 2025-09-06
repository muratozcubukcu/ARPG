#include "ps1_camera.h"
#include <algorithm>
#include <cmath>

namespace VKFW {
namespace Core {

PS1Camera::PS1Camera() : Camera() {
    setup_ps1_style();
}

PS1Camera::PS1Camera(const std::string& name) : Camera(name) {
    setup_ps1_style();
}

void PS1Camera::setup_ps1_style() {
    // Configure PS1-style camera settings
    m_ps1Settings.fov = 60.0f;
    m_ps1Settings.nearPlane = 0.1f;
    m_ps1Settings.farPlane = 100.0f;
    m_ps1Settings.affineProjection = true;
    m_ps1Settings.vertexFog = true;
    m_ps1Settings.fogStart = 0.5f;
    m_ps1Settings.fogEnd = 1.0f;
    m_ps1Settings.fogDensity = 1.0f;
    m_ps1Settings.fogColor = Vec3(0.5f, 0.5f, 0.5f);
    m_ps1Settings.mouseSensitivity = 0.1f;
    m_ps1Settings.maxPitch = 89.0f;
    m_ps1Settings.minPitch = -89.0f;
    m_ps1Settings.invertY = false;
    
    // Set camera properties
    m_fov = m_ps1Settings.fov;
    m_near = m_ps1Settings.nearPlane;
    m_far = m_ps1Settings.farPlane;
    
    // Initialize rotation
    m_currentYaw = 0.0f;
    m_currentPitch = 0.0f;
    
    update_ps1_projection();
}

void PS1Camera::set_ps1_settings(const PS1CameraSettings& settings) {
    m_ps1Settings = settings;
    update_ps1_projection();
}

void PS1Camera::update_first_person(float yaw, float pitch, const Vec3& position) {
    m_currentYaw = yaw;
    m_currentPitch = pitch;
    clamp_pitch();
    
    // Set position
    m_transform.position = position;
    
    // Calculate rotation from yaw and pitch
    Quat yawQuat = Quat::from_axis_angle(Vec3(0, 1, 0), math::radians(m_currentYaw));
    Quat pitchQuat = Quat::from_axis_angle(Vec3(1, 0, 0), math::radians(m_currentPitch));
    m_transform.rotation = yawQuat * pitchQuat;
    
    // Update camera vectors
    m_transform.forward = m_transform.rotation * Vec3(0, 0, -1);
    m_transform.right = math::cross(m_transform.forward, Vec3(0, 1, 0));
    m_transform.up = math::cross(m_transform.right, m_transform.forward);
    
    update_view_matrix();
    update_ps1_frustum();
}

void PS1Camera::process_mouse_movement(float xOffset, float yOffset) {
    xOffset *= m_ps1Settings.mouseSensitivity;
    yOffset *= m_ps1Settings.mouseSensitivity;
    
    if (m_ps1Settings.invertY) {
        yOffset = -yOffset;
    }
    
    m_currentYaw += xOffset;
    m_currentPitch += yOffset;
    
    clamp_pitch();
    
    // Update camera rotation
    Quat yawQuat = Quat::from_axis_angle(Vec3(0, 1, 0), math::radians(m_currentYaw));
    Quat pitchQuat = Quat::from_axis_angle(Vec3(1, 0, 0), math::radians(m_currentPitch));
    m_transform.rotation = yawQuat * pitchQuat;
    
    // Update camera vectors
    m_transform.forward = m_transform.rotation * Vec3(0, 0, -1);
    m_transform.right = math::cross(m_transform.forward, Vec3(0, 1, 0));
    m_transform.up = math::cross(m_transform.right, m_transform.forward);
    
    update_view_matrix();
    update_ps1_frustum();
}

void PS1Camera::process_keyboard_input(int key, float deltaTime) {
    // This would handle keyboard input for camera movement
    // Integration with your input system
}

void PS1Camera::set_fog_settings(float start, float end, float density, const Vec3& color) {
    m_ps1Settings.fogStart = start;
    m_ps1Settings.fogEnd = end;
    m_ps1Settings.fogDensity = density;
    m_ps1Settings.fogColor = color;
}

void PS1Camera::update_ps1_projection() {
    calculate_ps1_projection_matrix();
    update_ps1_frustum();
}

void PS1Camera::apply_ps1_style_rendering() {
    // Apply PS1-specific rendering settings
    // This would integrate with your rendering system
}

void PS1Camera::set_frustum() {
    // Override base frustum calculation for PS1-style
    update_ps1_frustum();
}

void PS1Camera::update_view_matrix() {
    // Calculate view matrix for first-person camera
    Vec3 target = m_transform.position + m_transform.forward;
    m_viewMatrix = math::look_at(m_transform.position, target, m_transform.up);
}

void PS1Camera::clamp_pitch() {
    m_currentPitch = std::clamp(m_currentPitch, m_ps1Settings.minPitch, m_ps1Settings.maxPitch);
}

void PS1Camera::update_ps1_frustum() {
    // PS1-style frustum calculation
    const float halfVSide = m_far * std::tan(math::radians(m_fov) * 0.5f);
    const float halfHSide = halfVSide * m_aspect;
    const Vec3 frontMultFar = m_far * m_transform.forward;
    
    m_frustrum.nearFace = {m_transform.position + m_near * m_transform.forward, m_transform.forward};
    m_frustrum.farFace = {m_transform.position + frontMultFar, -m_transform.forward};
    m_frustrum.rightFace = {m_transform.position,
                            math::cross(frontMultFar - m_transform.right * halfHSide, m_transform.up)};
    m_frustrum.leftFace = {m_transform.position,
                           math::cross(m_transform.up, frontMultFar + m_transform.right * halfHSide)};
    m_frustrum.topFace = {m_transform.position,
                          math::cross(m_transform.right, frontMultFar - m_transform.up * halfVSide)};
    m_frustrum.bottomFace = {m_transform.position,
                             math::cross(frontMultFar + m_transform.up * halfVSide, m_transform.right)};
}

void PS1Camera::calculate_ps1_projection_matrix() {
    // PS1-style projection matrix calculation
    // This would create a projection matrix suitable for PS1-style rendering
    // with the specified FOV, near, and far planes
    
    float fovRad = math::radians(m_fov);
    float tanHalfFov = std::tan(fovRad * 0.5f);
    
    m_projMatrix = Mat4::zero();
    m_projMatrix[0][0] = 1.0f / (m_aspect * tanHalfFov);
    m_projMatrix[1][1] = 1.0f / tanHalfFov;
    m_projMatrix[2][2] = -(m_far + m_near) / (m_far - m_near);
    m_projMatrix[2][3] = -1.0f;
    m_projMatrix[3][2] = -(2.0f * m_far * m_near) / (m_far - m_near);
}

} // namespace Core
} // namespace VKFW
