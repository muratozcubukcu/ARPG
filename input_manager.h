#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <map>
#include <vector>
#include <functional>
#include <chrono>
#include <string>

// Forward declarations
class PlayerController;

// Input event types
enum class InputEventType {
    KEY_PRESS,
    KEY_RELEASE,
    KEY_HOLD,
    MOUSE_MOVE,
    MOUSE_BUTTON_PRESS,
    MOUSE_BUTTON_RELEASE,
    MOUSE_WHEEL
};

// Input event structure
struct InputEvent {
    InputEventType type;
    int keyCode;
    float value;
    float deltaX;
    float deltaY;
    std::chrono::steady_clock::time_point timestamp;
    
    InputEvent(InputEventType t, int key = 0, float val = 0.0f, float dx = 0.0f, float dy = 0.0f)
        : type(t), keyCode(key), value(val), deltaX(dx), deltaY(dy), 
          timestamp(std::chrono::steady_clock::now()) {}
};

// Key codes (platform independent)
enum class KeyCode {
    // Movement keys
    W = 87, S = 83, A = 65, D = 68,
    SPACE = 32, SHIFT = 16, CTRL = 17,
    
    // Action keys
    E = 69, Q = 81, R = 82, F = 70,
    
    // Number keys
    ONE = 49, TWO = 50, THREE = 51, FOUR = 52, FIVE = 53,
    
    // Function keys
    F1 = 112, F2 = 113, F3 = 114, F4 = 115,
    
    // Special keys
    ESCAPE = 27, TAB = 9, ENTER = 13,
    
    // Mouse buttons
    MOUSE_LEFT = 1, MOUSE_RIGHT = 2, MOUSE_MIDDLE = 3
};

class InputManager {
private:
    std::map<int, bool> keyStates;
    std::map<int, bool> previousKeyStates;
    std::map<int, std::chrono::steady_clock::time_point> keyPressTimes;
    
    // Mouse state
    float mouseX, mouseY;
    float mouseDeltaX, mouseDeltaY;
    bool mouseLeftPressed, mouseRightPressed, mouseMiddlePressed;
    
    // Input callbacks
    std::map<int, std::function<void(bool)>> keyCallbacks;
    std::function<void(float, float)> mouseMoveCallback;
    std::function<void(int, bool)> mouseButtonCallback;
    
    // Input configuration
    float mouseSensitivity;
    bool mouseInverted;
    bool rawInputEnabled;
    
    // Timing
    std::chrono::steady_clock::time_point lastUpdateTime;
    float deltaTime;
    
public:
    InputManager();
    ~InputManager();
    
    // Core input processing
    void update();
    void processKeyEvent(int keyCode, bool pressed);
    void processMouseMove(float deltaX, float deltaY);
    void processMouseButton(int button, bool pressed);
    void processMouseWheel(float delta);
    
    // Key state queries
    bool isKeyPressed(int keyCode) const;
    bool isKeyJustPressed(int keyCode) const;
    bool isKeyJustReleased(int keyCode) const;
    bool isKeyHeld(int keyCode, float holdTime = 0.5f) const;
    
    // Mouse state queries
    float getMouseX() const { return mouseX; }
    float getMouseY() const { return mouseY; }
    float getMouseDeltaX() const { return mouseDeltaX; }
    float getMouseDeltaY() const { return mouseDeltaY; }
    bool isMouseButtonPressed(int button) const;
    
    // Input binding
    void bindKey(int keyCode, std::function<void(bool)> callback);
    void bindMouseMove(std::function<void(float, float)> callback);
    void bindMouseButton(std::function<void(int, bool)> callback);
    void unbindKey(int keyCode);
    
    // Configuration
    void setMouseSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }
    void setMouseInverted(bool inverted) { mouseInverted = inverted; }
    void setRawInputEnabled(bool enabled) { rawInputEnabled = enabled; }
    
    // Utility methods
    void resetMouseDelta();
    void clearAllBindings();
    float getDeltaTime() const { return deltaTime; }
    
    // Input recording/replay (for future use)
    void startRecording();
    void stopRecording();
    void replayInput(const std::string& filename);
    
private:
    // Helper methods
    void updateKeyStates();
    void updateMouseState();
    void processCallbacks();
    float getKeyHoldTime(int keyCode) const;
    
    // Input recording state
    bool isRecording;
    std::vector<InputEvent> recordedEvents;
};

#endif // INPUT_MANAGER_H
