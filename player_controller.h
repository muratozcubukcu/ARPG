#ifndef PLAYER_CONTROLLER_H
#define PLAYER_CONTROLLER_H

#include "position.h"
#include "character.h"
#include <memory>
#include <chrono>

// Forward declarations
class Camera;
class InputManager;

// Player movement state
struct PlayerMovementState {
    bool isMoving;
    bool isSprinting;
    bool isCrouching;
    bool isJumping;
    bool isGrounded;
    
    Position velocity;
    Position acceleration;
    float currentSpeed;
    float sprintMultiplier;
    float crouchMultiplier;
    float jumpForce;
    float gravity;
    
    PlayerMovementState() : isMoving(false), isSprinting(false), isCrouching(false),
                           isJumping(false), isGrounded(true), velocity(0, 0, 0),
                           acceleration(0, 0, 0), currentSpeed(0.0f), sprintMultiplier(1.5f),
                           crouchMultiplier(0.6f), jumpForce(8.0f), gravity(-20.0f) {}
};

class PlayerController {
private:
    Character* playerCharacter;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<InputManager> inputManager;
    
    PlayerMovementState movementState;
    
    // Movement constants
    const float BASE_MOVEMENT_SPEED = 5.0f;
    const float MAX_MOVEMENT_SPEED = 12.0f;
    const float ACCELERATION_RATE = 15.0f;
    const float DECELERATION_RATE = 20.0f;
    const float MOUSE_SENSITIVITY = 0.002f;
    const float MAX_LOOK_UP = 89.0f;  // Degrees
    const float MAX_LOOK_DOWN = -89.0f; // Degrees
    
    // Timing
    std::chrono::steady_clock::time_point lastFrameTime;
    float deltaTime;
    
    // Input state
    bool isMovingForward;
    bool isMovingBackward;
    bool isMovingLeft;
    bool isMovingRight;
    bool jumpPressed;
    bool sprintPressed;
    bool crouchPressed;
    
    // Camera angles
    float pitch;  // Up/down rotation
    float yaw;    // Left/right rotation
    
public:
    PlayerController(Character* character = nullptr);
    ~PlayerController();
    
    // Core update loop
    void update();
    void processInput();
    void updateMovement(float dt);
    void updateCamera(float dt);
    
    // Movement methods
    void moveForward(bool pressed);
    void moveBackward(bool pressed);
    void moveLeft(bool pressed);
    void moveRight(bool pressed);
    void jump(bool pressed);
    void sprint(bool pressed);
    void crouch(bool pressed);
    void lookMouse(float deltaX, float deltaY);
    
    // Physics and collision
    void applyGravity(float dt);
    void checkGroundCollision();
    void resolveCollisions();
    
    // Getters
    Character* getPlayerCharacter() const { return playerCharacter; }
    Camera* getCamera() const { return camera.get(); }
    const PlayerMovementState& getMovementState() const { return movementState; }
    Position getPlayerPosition() const;
    Position getCameraPosition() const;
    
    // Setters
    void setPlayerCharacter(Character* character) { playerCharacter = character; }
    void setMouseSensitivity(float sensitivity) { /* TODO: Implement */ }
    void setMovementSpeed(float speed) { /* TODO: Implement */ }
    
    // Utility
    void resetCamera();
    void teleportPlayer(const Position& newPos);
    bool isPlayerMoving() const { return movementState.isMoving; }
};

#endif // PLAYER_CONTROLLER_H
