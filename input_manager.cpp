#include "input_manager.h"
#include <iostream>

InputManager::InputManager()
    : mouseX(0.0f), mouseY(0.0f), mouseDeltaX(0.0f), mouseDeltaY(0.0f),
      mouseLeftPressed(false), mouseRightPressed(false), mouseMiddlePressed(false),
      mouseSensitivity(1.0f), mouseInverted(false), rawInputEnabled(false),
      deltaTime(0.0f), isRecording(false) {
    
    std::cout << "InputManager initialized" << std::endl;
    lastUpdateTime = std::chrono::steady_clock::now();
}

InputManager::~InputManager() {
    std::cout << "InputManager destroyed" << std::endl;
}

void InputManager::update() {
    // Calculate delta time
    auto currentTime = std::chrono::steady_clock::now();
    deltaTime = std::chrono::duration<float>(currentTime - lastUpdateTime).count();
    lastUpdateTime = currentTime;
    
    // Update key states
    updateKeyStates();
    updateMouseState();
    
    // Process callbacks
    processCallbacks();
    
    // Reset mouse delta for next frame
    resetMouseDelta();
}

void InputManager::processKeyEvent(int keyCode, bool pressed) {
    keyStates[keyCode] = pressed;
    
    if (pressed) {
        keyPressTimes[keyCode] = std::chrono::steady_clock::now();
    }
    
    // Trigger callback if bound
    auto it = keyCallbacks.find(keyCode);
    if (it != keyCallbacks.end()) {
        it->second(pressed);
    }
    
    // Record input if recording
    if (isRecording) {
        InputEvent event(pressed ? InputEventType::KEY_PRESS : InputEventType::KEY_RELEASE, keyCode);
        recordedEvents.push_back(event);
    }
}

void InputManager::processMouseMove(float deltaX, float deltaY) {
    mouseDeltaX = deltaX * mouseSensitivity;
    mouseDeltaY = deltaY * mouseSensitivity;
    
    if (mouseInverted) {
        mouseDeltaY = -mouseDeltaY;
    }
    
    mouseX += mouseDeltaX;
    mouseY += mouseDeltaY;
    
    // Trigger mouse move callback
    if (mouseMoveCallback) {
        mouseMoveCallback(mouseDeltaX, mouseDeltaY);
    }
    
    // Record input if recording
    if (isRecording) {
        InputEvent event(InputEventType::MOUSE_MOVE, 0, 0.0f, deltaX, deltaY);
        recordedEvents.push_back(event);
    }
}

void InputManager::processMouseButton(int button, bool pressed) {
    switch (button) {
        case 1: mouseLeftPressed = pressed; break;
        case 2: mouseRightPressed = pressed; break;
        case 3: mouseMiddlePressed = pressed; break;
    }
    
    // Trigger mouse button callback
    if (mouseButtonCallback) {
        mouseButtonCallback(button, pressed);
    }
    
    // Record input if recording
    if (isRecording) {
        InputEvent event(pressed ? InputEventType::MOUSE_BUTTON_PRESS : InputEventType::MOUSE_BUTTON_RELEASE, button);
        recordedEvents.push_back(event);
    }
}

void InputManager::processMouseWheel(float delta) {
    // Record input if recording
    if (isRecording) {
        InputEvent event(InputEventType::MOUSE_WHEEL, 0, delta);
        recordedEvents.push_back(event);
    }
}

bool InputManager::isKeyPressed(int keyCode) const {
    auto it = keyStates.find(keyCode);
    return it != keyStates.end() && it->second;
}

bool InputManager::isKeyJustPressed(int keyCode) const {
    auto it = keyStates.find(keyCode);
    auto prevIt = previousKeyStates.find(keyCode);
    
    return it != keyStates.end() && it->second && 
           (prevIt == previousKeyStates.end() || !prevIt->second);
}

bool InputManager::isKeyJustReleased(int keyCode) const {
    auto it = keyStates.find(keyCode);
    auto prevIt = previousKeyStates.find(keyCode);
    
    return it != keyStates.end() && !it->second && 
           (prevIt != previousKeyStates.end() && prevIt->second);
}

bool InputManager::isKeyHeld(int keyCode, float holdTime) const {
    auto it = keyPressTimes.find(keyCode);
    if (it == keyPressTimes.end()) return false;
    
    auto currentTime = std::chrono::steady_clock::now();
    float heldTime = std::chrono::duration<float>(currentTime - it->second).count();
    
    return heldTime >= holdTime;
}

bool InputManager::isMouseButtonPressed(int button) const {
    switch (button) {
        case 1: return mouseLeftPressed;
        case 2: return mouseRightPressed;
        case 3: return mouseMiddlePressed;
        default: return false;
    }
}

void InputManager::bindKey(int keyCode, std::function<void(bool)> callback) {
    keyCallbacks[keyCode] = callback;
}

void InputManager::bindMouseMove(std::function<void(float, float)> callback) {
    mouseMoveCallback = callback;
}

void InputManager::bindMouseButton(std::function<void(int, bool)> callback) {
    mouseButtonCallback = callback;
}

void InputManager::unbindKey(int keyCode) {
    keyCallbacks.erase(keyCode);
}

void InputManager::resetMouseDelta() {
    mouseDeltaX = 0.0f;
    mouseDeltaY = 0.0f;
}

void InputManager::clearAllBindings() {
    keyCallbacks.clear();
    mouseMoveCallback = nullptr;
    mouseButtonCallback = nullptr;
}

void InputManager::startRecording() {
    isRecording = true;
    recordedEvents.clear();
    std::cout << "Input recording started" << std::endl;
}

void InputManager::stopRecording() {
    isRecording = false;
    std::cout << "Input recording stopped. Recorded " << recordedEvents.size() << " events." << std::endl;
}

void InputManager::replayInput(const std::string& filename) {
    // TODO: Implement input replay from file
    std::cout << "Input replay not yet implemented" << std::endl;
}

void InputManager::updateKeyStates() {
    previousKeyStates = keyStates;
}

void InputManager::updateMouseState() {
    // Mouse state is updated in processMouseMove and processMouseButton
}

void InputManager::processCallbacks() {
    // Callbacks are processed immediately in the event methods
}

float InputManager::getKeyHoldTime(int keyCode) const {
    auto it = keyPressTimes.find(keyCode);
    if (it == keyPressTimes.end()) return 0.0f;
    
    auto currentTime = std::chrono::steady_clock::now();
    return std::chrono::duration<float>(currentTime - it->second).count();
}
