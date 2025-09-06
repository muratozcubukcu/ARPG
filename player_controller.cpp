#include "player_controller.h"
#include "camera.h"
#include "input_manager.h"
#include <iostream>
#include <algorithm>

PlayerController::PlayerController(Character* character)
    : playerCharacter(character), camera(std::make_unique<Camera>()), 
      inputManager(std::make_unique<InputManager>()), deltaTime(0.0f),
      isMovingForward(false), isMovingBackward(false), isMovingLeft(false), isMovingRight(false),
      jumpPressed(false), sprintPressed(false), crouchPressed(false),
      pitch(0.0f), yaw(0.0f) {
    
    std::cout << "PlayerController initialized" << std::endl;
    
    // Set up input bindings
    if (inputManager) {
        inputManager->bindKey(static_cast<int>(KeyCode::W), 
            [this](bool pressed) { moveForward(pressed); });
        inputManager->bindKey(static_cast<int>(KeyCode::S), 
            [this](bool pressed) { moveBackward(pressed); });
        inputManager->bindKey(static_cast<int>(KeyCode::A), 
            [this](bool pressed) { moveLeft(pressed); });
        inputManager->bindKey(static_cast<int>(KeyCode::D), 
            [this](bool pressed) { moveRight(pressed); });
        inputManager->bindKey(static_cast<int>(KeyCode::SPACE), 
            [this](bool pressed) { jump(pressed); });
        inputManager->bindKey(static_cast<int>(KeyCode::SHIFT), 
            [this](bool pressed) { sprint(pressed); });
        inputManager->bindKey(static_cast<int>(KeyCode::CTRL), 
            [this](bool pressed) { crouch(pressed); });
        
        // Mouse movement binding
        inputManager->bindMouseMove([this](float deltaX, float deltaY) {
            lookMouse(deltaX, deltaY);
        });
    }
}

PlayerController::~PlayerController() {
    std::cout << "PlayerController destroyed" << std::endl;
}

void PlayerController::update() {
    // Calculate delta time
    auto currentTime = std::chrono::steady_clock::now();
    deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
    lastFrameTime = currentTime;
    
    // Clamp delta time to prevent spiral of death
    if (deltaTime > 0.1f) deltaTime = 0.1f;
    
    // Process input and update systems
    processInput();
    updateMovement(deltaTime);
    updateCamera(deltaTime);
    
    // Apply physics
    applyGravity(deltaTime);
    checkGroundCollision();
    resolveCollisions();
}

void PlayerController::processInput() {
    if (inputManager) {
        inputManager->update();
    }
}

void PlayerController::updateMovement(float dt) {
    if (!playerCharacter) return;
    
    // Calculate movement direction based on input
    Position moveDirection(0, 0, 0);
    
    if (isMovingForward) moveDirection = moveDirection + camera->getForwardVector();
    if (isMovingBackward) moveDirection = moveDirection - camera->getForwardVector();
    if (isMovingLeft) moveDirection = moveDirection - camera->getRightVector();
    if (isMovingRight) moveDirection = moveDirection + camera->getRightVector();
    
    // Normalize movement direction
    if (moveDirection.length() > 0) {
        moveDirection = moveDirection.normalize();
        movementState.isMoving = true;
        
        // Calculate movement speed
        float currentSpeed = BASE_MOVEMENT_SPEED;
        if (movementState.isSprinting) currentSpeed *= movementState.sprintMultiplier;
        if (movementState.isCrouching) currentSpeed *= movementState.crouchMultiplier;
        
        // Apply movement
        Position movement = moveDirection * (currentSpeed * dt);
        Position currentPos = playerCharacter->getPosition();
        playerCharacter->setPosition(currentPos + movement);
        
        // Update camera position to follow player
        if (camera) {
            Position cameraOffset(0, 0, 1.7f); // Eye level height
            camera->setPosition(currentPos + cameraOffset);
        }
    } else {
        movementState.isMoving = false;
    }
}

void PlayerController::updateCamera(float dt) {
    if (!camera) return;
    
    // Update camera rotation based on mouse input
    camera->setRotation(pitch, yaw);
}

void PlayerController::moveForward(bool pressed) {
    isMovingForward = pressed;
}

void PlayerController::moveBackward(bool pressed) {
    isMovingBackward = pressed;
}

void PlayerController::moveLeft(bool pressed) {
    isMovingLeft = pressed;
}

void PlayerController::moveRight(bool pressed) {
    isMovingRight = pressed;
}

void PlayerController::jump(bool pressed) {
    if (pressed && !jumpPressed && movementState.isGrounded) {
        movementState.isJumping = true;
        movementState.velocity.setZ(movementState.jumpForce);
        movementState.isGrounded = false;
    }
    jumpPressed = pressed;
}

void PlayerController::sprint(bool pressed) {
    movementState.isSprinting = pressed;
}

void PlayerController::crouch(bool pressed) {
    movementState.isCrouching = pressed;
}

void PlayerController::lookMouse(float deltaX, float deltaY) {
    // Update camera angles
    yaw += deltaX * MOUSE_SENSITIVITY;
    pitch -= deltaY * MOUSE_SENSITIVITY;
    
    // Clamp pitch to prevent over-rotation
    if (pitch > MAX_LOOK_UP * 3.14159265358979323846f / 180.0f) pitch = MAX_LOOK_UP * 3.14159265358979323846f / 180.0f;
    if (pitch < MAX_LOOK_DOWN * 3.14159265358979323846f / 180.0f) pitch = MAX_LOOK_DOWN * 3.14159265358979323846f / 180.0f;
}

void PlayerController::applyGravity(float dt) {
    if (!movementState.isGrounded) {
        movementState.velocity.setZ(movementState.velocity.getZ() + movementState.gravity * dt);
        
        // Apply velocity to position
        if (playerCharacter) {
            Position currentPos = playerCharacter->getPosition();
            Position gravityMovement(0, 0, movementState.velocity.getZ() * dt);
            playerCharacter->setPosition(currentPos + gravityMovement);
        }
    }
}

void PlayerController::checkGroundCollision() {
    if (!playerCharacter) return;
    
    // Simple ground collision check (Y = 0)
    Position currentPos = playerCharacter->getPosition();
    if (currentPos.getZ() <= 0.0f) {
        playerCharacter->setPosition(currentPos.getX(), currentPos.getY(), 0.0f);
        movementState.isGrounded = true;
        movementState.isJumping = false;
        movementState.velocity.setZ(0.0f);
    }
}

void PlayerController::resolveCollisions() {
    // Enhanced collision resolution with physics system integration
    if (!playerCharacter) return;
    
    Position playerPos = playerCharacter->getPosition();
    
    // Ground collision
    if (playerPos.getZ() < 0.0f) {
        playerCharacter->setPosition(playerPos.getX(), playerPos.getY(), 0.0f);
        
        // Reset vertical velocity when hitting ground
        if (movementState.velocity.getZ() < 0) {
            movementState.velocity.setZ(0);
            movementState.isGrounded = true;
            movementState.isJumping = false;
        }
    }
    
    // Wall collision (simple boundary checking)
    const float worldBounds = 100.0f; // Match physics system bounds
    if (std::abs(playerPos.getX()) > worldBounds) {
        float clampedX = playerPos.getX();
        if (clampedX < -worldBounds) clampedX = -worldBounds;
        if (clampedX > worldBounds) clampedX = worldBounds;
        playerCharacter->setPosition(clampedX, playerPos.getY(), playerPos.getZ());
        movementState.velocity.setX(0); // Stop horizontal movement
    }
    
    if (std::abs(playerPos.getY()) > worldBounds) {
        float clampedY = playerPos.getY();
        if (clampedY < -worldBounds) clampedY = -worldBounds;
        if (clampedY > worldBounds) clampedY = worldBounds;
        playerPos = playerCharacter->getPosition(); // Get updated position
        playerCharacter->setPosition(playerPos.getX(), clampedY, playerPos.getZ());
        movementState.velocity.setY(0); // Stop horizontal movement
    }
    
    // Ceiling collision (prevent flying too high)
    const float maxHeight = 50.0f;
    if (playerPos.getZ() > maxHeight) {
        playerCharacter->setPosition(playerPos.getX(), playerPos.getY(), maxHeight);
        movementState.velocity.setZ(0);
    }
}

Position PlayerController::getPlayerPosition() const {
    return playerCharacter ? playerCharacter->getPosition() : Position(0, 0, 0);
}

Position PlayerController::getCameraPosition() const {
    return camera ? camera->getPosition() : Position(0, 0, 0);
}

void PlayerController::resetCamera() {
    pitch = 0.0f;
    yaw = 0.0f;
    if (camera) {
        camera->resetRotation();
    }
}

void PlayerController::teleportPlayer(const Position& newPos) {
    if (playerCharacter) {
        playerCharacter->setPosition(newPos);
        if (camera) {
            Position cameraOffset(0, 0, 1.7f);
            camera->setPosition(newPos + cameraOffset);
        }
    }
}
