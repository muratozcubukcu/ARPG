#include "camera.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Camera::Camera(const Position& pos)
    : position(pos), target(0, 0, 0), up(0, 0, 1), pitch(0.0f), yaw(0.0f),
      fov(70.0f), aspectRatio(16.0f/9.0f), nearPlane(0.1f), farPlane(1000.0f),
      minPitch(-89.0f * M_PI / 180.0f), maxPitch(89.0f * M_PI / 180.0f),
      shakeIntensity(0.0f), shakeDuration(0.0f), shakeTimer(0.0f), shakeOffset(0, 0, 0) {
    
    updateVectors();
}

void Camera::update() {
    updateShake(0.016f); // Assume 60 FPS
    updateVectors();
}

void Camera::lookAt(const Position& newTarget) {
    target = newTarget;
    updateVectors();
}

void Camera::setPosition(const Position& pos) {
    position = pos;
    updateVectors();
}

void Camera::setRotation(float newPitch, float newYaw) {
    pitch = newPitch;
    yaw = newYaw;
    clampPitch();
    updateVectors();
}

void Camera::moveForward(float distance) {
    Position forward = getForwardVector();
    position = position + forward * distance;
    updateVectors();
}

void Camera::moveRight(float distance) {
    Position right = getRightVector();
    position = position + right * distance;
    updateVectors();
}

void Camera::moveUp(float distance) {
    Position upVec = getUpVector();
    position = position + upVec * distance;
    updateVectors();
}

void Camera::rotate(float deltaPitch, float deltaYaw) {
    pitch += deltaPitch;
    yaw += deltaYaw;
    clampPitch();
    updateVectors();
}

void Camera::calculateViewMatrix() {
    // This would calculate the view matrix for rendering
    // For now, we just update the vectors
    updateVectors();
}

void Camera::calculateProjectionMatrix() {
    // This would calculate the projection matrix for rendering
    // For now, it's a placeholder
}

Position Camera::getForwardVector() const {
    float x = cos(pitch) * sin(yaw);
    float y = cos(pitch) * cos(yaw);
    float z = sin(pitch);
    return Position(x, y, z);
}

Position Camera::getRightVector() const {
    float x = cos(yaw);
    float y = -sin(yaw);
    float z = 0.0f;
    return Position(x, y, z);
}

Position Camera::getUpVector() const {
    float x = -sin(pitch) * sin(yaw);
    float y = -sin(pitch) * cos(yaw);
    float z = cos(pitch);
    return Position(x, y, z);
}

void Camera::resetRotation() {
    pitch = 0.0f;
    yaw = 0.0f;
    updateVectors();
}

void Camera::clampPitch() {
    if (pitch > maxPitch) pitch = maxPitch;
    if (pitch < minPitch) pitch = minPitch;
}

void Camera::addShake(float intensity, float duration) {
    shakeIntensity = intensity;
    shakeDuration = duration;
    shakeTimer = 0.0f;
}

void Camera::updateShake(float deltaTime) {
    if (shakeTimer < shakeDuration) {
        shakeTimer += deltaTime;
        
        // Simple random shake effect
        float progress = shakeTimer / shakeDuration;
        float currentIntensity = shakeIntensity * (1.0f - progress);
        
        shakeOffset.setX((rand() % 100 - 50) / 100.0f * currentIntensity);
        shakeOffset.setY((rand() % 100 - 50) / 100.0f * currentIntensity);
        shakeOffset.setZ((rand() % 100 - 50) / 100.0f * currentIntensity);
    } else {
        shakeOffset = Position(0, 0, 0);
    }
}

void Camera::updateVectors() {
    // Calculate forward vector
    Position forward = getForwardVector();
    
    // Calculate target position
    target = position + forward;
    
    // Calculate up vector
    up = getUpVector();
}
