#ifndef CAMERA_H
#define CAMERA_H

#include "position.h"
#include <cmath>

class Camera {
private:
    Position position;
    Position target;
    Position up;
    
    // Camera orientation
    float pitch;  // Up/down rotation (radians)
    float yaw;    // Left/right rotation (radians)
    
    // Camera properties
    float fov;           // Field of view in degrees
    float aspectRatio;   // Width/height ratio
    float nearPlane;     // Near clipping plane
    float farPlane;      // Far clipping plane
    
    // Movement constraints
    float minPitch;
    float maxPitch;
    
public:
    Camera(const Position& pos = Position(0, 0, 0));
    ~Camera() = default;
    
    // Core camera methods
    void update();
    void lookAt(const Position& target);
    void setPosition(const Position& pos);
    void setRotation(float newPitch, float newYaw);
    
    // Movement methods
    void moveForward(float distance);
    void moveRight(float distance);
    void moveUp(float distance);
    void rotate(float deltaPitch, float deltaYaw);
    
    // View matrix calculations
    void calculateViewMatrix();
    void calculateProjectionMatrix();
    
    // Getters
    Position getPosition() const { return position; }
    Position getTarget() const { return target; }
    Position getUp() const { return up; }
    float getPitch() const { return pitch; }
    float getYaw() const { return yaw; }
    float getFOV() const { return fov; }
    
    // Setters
    void setFOV(float newFOV) { fov = newFOV; }
    void setAspectRatio(float ratio) { aspectRatio = ratio; }
    void setNearPlane(float near) { nearPlane = near; }
    void setFarPlane(float far) { farPlane = far; }
    
    // Utility methods
    Position getForwardVector() const;
    Position getRightVector() const;
    Position getUpVector() const;
    void resetRotation();
    void clampPitch();
    
    // Camera shake and effects
    void addShake(float intensity, float duration);
    void updateShake(float deltaTime);
    
private:
    // Camera shake state
    float shakeIntensity;
    float shakeDuration;
    float shakeTimer;
    Position shakeOffset;
    
    // Helper methods
    void updateVectors();
    float toRadians(float degrees) const { return degrees * 3.14159265358979323846f / 180.0f; }
    float toDegrees(float radians) const { return radians * 180.0f / 3.14159265358979323846f; }
};

#endif // CAMERA_H
