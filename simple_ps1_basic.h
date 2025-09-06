#pragma once

#define _USE_MATH_DEFINES
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <GL/gl.h>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include <iostream>

namespace SimplePS1 {

// Simple vector and matrix classes
struct Vec3 {
    float x, y, z;
    
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    
    Vec3 operator+(const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
    Vec3 operator-(const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
    Vec3 operator*(float scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
    Vec3 operator/(float scalar) const { return Vec3(x / scalar, y / scalar, z / scalar); }
    
    float length() const { return std::sqrt(x*x + y*y + z*z); }
    Vec3 normalize() const { float len = length(); return len > 0 ? *this / len : Vec3(0,0,0); }
    float dot(const Vec3& other) const { return x*other.x + y*other.y + z*other.z; }
    Vec3 cross(const Vec3& other) const { 
        return Vec3(y*other.z - z*other.y, z*other.x - x*other.z, x*other.y - y*other.x); 
    }
};

struct Vec2 {
    float x, y;
    
    Vec2() : x(0), y(0) {}
    Vec2(float x, float y) : x(x), y(y) {}
};

struct PS1Vertex {
    Vec3 position;
    Vec2 texCoord;
    Vec3 normal;
    Vec3 color; // For vertex colors
    
    PS1Vertex(const Vec3& pos, const Vec2& tex, const Vec3& norm, const Vec3& col = Vec3(1.0f, 1.0f, 1.0f))
        : position(pos), texCoord(tex), normal(norm), color(col) {}
};

struct PS1Mesh {
    std::vector<PS1Vertex> vertices;
    std::vector<unsigned int> indices;
    
    void render() {
        glBegin(GL_TRIANGLES);
        for (size_t i = 0; i < indices.size(); i += 3) {
            // Get triangle vertices
            const PS1Vertex& v1 = vertices[indices[i]];
            const PS1Vertex& v2 = vertices[indices[i+1]];
            const PS1Vertex& v3 = vertices[indices[i+2]];
            
            // Calculate normal for flat shading
            Vec3 edge1 = v2.position - v1.position;
            Vec3 edge2 = v3.position - v1.position;
            Vec3 normal = edge1.cross(edge2).normalize();
            
            // Simple lighting calculation
            Vec3 lightDir = Vec3(0.5f, 1.0f, 0.3f).normalize();
            float lightIntensity = std::max(0.0f, normal.dot(lightDir));
            
            // PS1-style quantized lighting
            lightIntensity = std::floor(lightIntensity * 4.0f) / 4.0f;
            
            // Apply lighting to color
            Vec3 litColor = v1.color * lightIntensity;
            
            // Render triangle
            glColor3f(litColor.x, litColor.y, litColor.z);
            glVertex3f(v1.position.x, v1.position.y, v1.position.z);
            glVertex3f(v2.position.x, v2.position.y, v2.position.z);
            glVertex3f(v3.position.x, v3.position.y, v3.position.z);
        }
        glEnd();
    }
};

class PS1Camera {
public:
    Vec3 position;
    float yaw, pitch;
    float fov, nearPlane, farPlane;
    int screenWidth, screenHeight;
    
    // Look direction vectors (updated when yaw/pitch changes)
    Vec3 forward;  // Direction camera is looking
    Vec3 right;    // Right direction relative to camera
    Vec3 up;       // Up direction (always (0,1,0) for now)
    
    PS1Camera(int width, int height) 
        : position(0, 0, 0), yaw(0), pitch(0), fov(60.0f), nearPlane(0.1f), farPlane(100.0f),
          screenWidth(width), screenHeight(height), forward(0, 0, -1), right(1, 0, 0), up(0, 1, 0) {
        // Initialize look direction vectors
        updateLookDirection();
    }
    
    void update(float deltaTime) {
        // Update look direction vectors based on current yaw/pitch
        updateLookDirection();
    }
    
    void updateLookDirection() {
        // Convert angles to radians
        float yawRad = yaw * M_PI / 180.0f;
        float pitchRad = pitch * M_PI / 180.0f;
        
        // Calculate forward vector (where camera is looking)
        forward = Vec3(
            std::sin(yawRad) * std::cos(pitchRad),
            -std::sin(pitchRad),
            -std::cos(yawRad) * std::cos(pitchRad)
        ).normalize();
        
        // Calculate right and up using a right-handed basis
        Vec3 worldUp(0, 1, 0);
        right = forward.cross(worldUp).normalize();   // <— changed order
        up    = right.cross(forward).normalize();     // <— changed order
        
        // Wrap yaw
        if (yaw > 360.f || yaw < -360.f) yaw = std::fmod(yaw, 360.f);
    }
    
    void setupProjection() {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        
        float aspect = (float)screenWidth / (float)screenHeight;
        float fovRad = fov * M_PI / 180.0f;
        float f = 1.0f / std::tan(fovRad * 0.5f);
        
        float proj[16] = {
            f/aspect, 0, 0, 0,
            0, f, 0, 0,
            0, 0, (farPlane + nearPlane) / (nearPlane - farPlane), -1,
            0, 0, (2 * farPlane * nearPlane) / (nearPlane - farPlane), 0
        };
        
        glLoadMatrixf(proj);
    }
    
    void setupView() {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Use the camera's basis you already maintain
        Vec3 f = forward.normalize();                 // view forward
        Vec3 r = f.cross(Vec3(0.f, 1.f, 0.f)).normalize(); // RIGHT = f × up
        Vec3 u = r.cross(f).normalize();              // true up

        // Row-major to column-major for OpenGL (column-major array)
        float m[16] = {
            r.x,  u.x,  -f.x,  0.f,
            r.y,  u.y,  -f.y,  0.f,
            r.z,  u.z,  -f.z,  0.f,
            -(r.x*position.x + r.y*position.y + r.z*position.z),
            -(u.x*position.x + u.y*position.y + u.z*position.z),
             (f.x*position.x + f.y*position.y + f.z*position.z),
             1.f
        };
        glLoadMatrixf(m);
    }
    
    // First-person controls
    void handleInput(const sf::Window& window, float deltaTime) {
        const float mouseSensitivity = 0.1f;
        const float moveSpeed = 5.0f;

        // --- 1) Mouse look FIRST ---
        static bool mouseCaptured = false;
        static sf::Vector2i lastMousePos;
        static bool firstMouse = true;

        if (sf::Mouse::isButtonPressed(sf::Mouse::Right) || sf::Mouse::isButtonPressed(sf::Mouse::Middle)) {
            if (!mouseCaptured) {
                mouseCaptured = true;
                lastMousePos = sf::Mouse::getPosition(window);
                firstMouse = true;
            }
        } else {
            mouseCaptured = false;
        }

        if (mouseCaptured) {
            sf::Vector2i currentMousePos = sf::Mouse::getPosition(window);
            if (!firstMouse) {
                sf::Vector2i delta = currentMousePos - lastMousePos;
                yaw   += delta.x * mouseSensitivity;
                pitch -= delta.y * mouseSensitivity;
                pitch = std::clamp(pitch, -89.0f, 89.0f);
                updateLookDirection();              // <-- update basis BEFORE movement
            }
            firstMouse = false;
            lastMousePos = currentMousePos;
        }

        // Build ground-projected basis from the UPDATED camera basis
        Vec3 flatFwd   = Vec3(forward.x, 0.f, forward.z);
        Vec3 flatRight = Vec3(right.x,   0.f, right.z);

        // Fallback if looking nearly straight up/down
        const float EPS = 1e-3f;
        if (flatFwd.length() < EPS) {
            float yawRad = yaw * M_PI / 180.0f;
            flatFwd = Vec3(std::sin(yawRad), 0.f, -std::cos(yawRad));
            // Keep right consistent with a RIGHT-handed basis on the ground:
            flatRight = Vec3(0.f,1.f,0.f).cross(flatFwd); // up × fwd = RIGHT
        }

        flatFwd   = flatFwd.normalize();
        flatRight = flatRight.normalize();

        // Debug logging for sanity checks (temporary)
        static bool wasMoving = false;
        bool isMoving = sf::Keyboard::isKeyPressed(sf::Keyboard::W) || 
                       sf::Keyboard::isKeyPressed(sf::Keyboard::A) || 
                       sf::Keyboard::isKeyPressed(sf::Keyboard::S) || 
                       sf::Keyboard::isKeyPressed(sf::Keyboard::D);
        
        if (isMoving && !wasMoving) {
            std::cout << "yaw=" << yaw
                      << " flatFwd=(" << flatFwd.x << "," << flatFwd.z << ")"
                      << " flatRight=(" << flatRight.x << "," << flatRight.z << ")\n";
        }
        wasMoving = isMoving;

        // --- 3) Accumulate WASD using the fresh basis ---
        Vec3 move(0,0,0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) move = move + flatFwd;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) move = move - flatFwd;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) move = move - flatRight; // left
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) move = move + flatRight; // right

        // Vertical (world up/down)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))  move.y += 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) move.y -= 1.f;

        // Normalize so diagonals aren't faster
        float len = move.length();
        if (len > 0.f) move = move / len;

        position = position + move * (moveSpeed * deltaTime);
    }
    
    void setPosition(const Vec3& pos) { position = pos; }
    void addRotation(float yawDelta, float pitchDelta) {
        yaw += yawDelta;
        pitch += pitchDelta;
        pitch = std::max(-89.0f, std::min(89.0f, pitch));
        updateLookDirection();
    }
};

class SimplePS1Renderer {
private:
    sf::RenderWindow window;
    PS1Camera camera;
    std::vector<std::unique_ptr<PS1Mesh>> meshes;
    
    // PS1-style settings
    bool enableVertexLighting = true;
    bool enableQuantizedLighting = true;
    bool enableFog = true;
    Vec3 fogColor = Vec3(0.5f, 0.5f, 0.5f);
    
    void setupOpenGL();
    
public:
    SimplePS1Renderer(int width = 1024, int height = 768, const std::string& title = "PS1 Style Renderer");
    ~SimplePS1Renderer();
    
    bool initialize();
    void shutdown();
    
    // Mesh management
    PS1Mesh* addMesh(const std::vector<PS1Vertex>& vertices, const std::vector<unsigned int>& indices);
    void clearMeshes();
    
    // Rendering
    void beginFrame();
    void renderFrame();
    void endFrame();
    
    // PS1 settings
    void setVertexLighting(bool enable) { enableVertexLighting = enable; }
    void setQuantizedLighting(bool enable) { enableQuantizedLighting = enable; }
    void setFog(bool enable) { enableFog = enable; }
    void setFogColor(const Vec3& color) { fogColor = color; }
    
    // Camera access
    PS1Camera& getCamera() { return camera; }
    
    // Window access
    sf::RenderWindow& getWindow() { return window; }
    bool isOpen() const { return window.isOpen(); }
    
    // Input handling
    void handleEvents();
    void handleInput(float deltaTime);
};

} // namespace SimplePS1
