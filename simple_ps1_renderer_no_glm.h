#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <GL/gl.h>
#include <vector>
#include <memory>
#include <cmath>

namespace SimplePS1 {

// Simple vector and matrix classes to replace GLM
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

struct Mat4 {
    float m[16];
    
    Mat4() {
        for(int i = 0; i < 16; i++) m[i] = 0;
        m[0] = m[5] = m[10] = m[15] = 1; // Identity matrix
    }
    
    static Mat4 identity() { return Mat4(); }
    
    static Mat4 translate(const Vec3& v) {
        Mat4 result = identity();
        result.m[12] = v.x;
        result.m[13] = v.y;
        result.m[14] = v.z;
        return result;
    }
    
    static Mat4 rotateX(float angle) {
        Mat4 result = identity();
        float c = std::cos(angle);
        float s = std::sin(angle);
        result.m[5] = c; result.m[6] = -s;
        result.m[9] = s; result.m[10] = c;
        return result;
    }
    
    static Mat4 rotateY(float angle) {
        Mat4 result = identity();
        float c = std::cos(angle);
        float s = std::sin(angle);
        result.m[0] = c; result.m[2] = s;
        result.m[8] = -s; result.m[10] = c;
        return result;
    }
    
    static Mat4 perspective(float fov, float aspect, float near, float far) {
        Mat4 result;
        float f = 1.0f / std::tan(fov * 0.5f);
        result.m[0] = f / aspect;
        result.m[5] = f;
        result.m[10] = (far + near) / (near - far);
        result.m[11] = -1;
        result.m[14] = (2 * far * near) / (near - far);
        result.m[15] = 0;
        return result;
    }
    
    Mat4 operator*(const Mat4& other) const {
        Mat4 result;
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                result.m[i*4+j] = 0;
                for(int k = 0; k < 4; k++) {
                    result.m[i*4+j] += m[i*4+k] * other.m[k*4+j];
                }
            }
        }
        return result;
    }
    
    const float* data() const { return m; }
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
    GLuint vao, vbo, ebo;
    bool uploaded = false;
    
    PS1Mesh() : vao(0), vbo(0), ebo(0) {}
    ~PS1Mesh() { cleanup(); }
    
    void upload();
    void render();
    void cleanup();
};

class PS1Camera {
public:
    Vec3 position;
    float yaw, pitch;
    float fov, nearPlane, farPlane;
    int screenWidth, screenHeight;
    
    PS1Camera(int width, int height);
    
    void update(float deltaTime);
    Mat4 getViewMatrix() const;
    Mat4 getProjectionMatrix() const;
    
    // First-person controls
    void handleInput(const sf::Window& window, float deltaTime);
    void setPosition(const Vec3& pos) { position = pos; }
    void addRotation(float yawDelta, float pitchDelta);
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
    bool enableDithering = true;
    float fogStart = 10.0f;
    float fogEnd = 50.0f;
    Vec3 fogColor = Vec3(0.5f, 0.5f, 0.5f);
    
    // Lighting
    Vec3 lightDirection = Vec3(0.0f, 1.0f, 0.0f);
    Vec3 lightColor = Vec3(1.0f, 1.0f, 1.0f);
    
    // Shader program
    GLuint shaderProgram;
    GLuint vertexShader, fragmentShader;
    
    // Uniform locations
    GLint uModel, uView, uProjection;
    GLint uLightDir, uLightColor;
    GLint uFogStart, uFogEnd, uFogColor;
    GLint uEnableVertexLighting, uEnableQuantizedLighting, uEnableFog, uEnableDithering;
    
    void setupShaders();
    void setupOpenGL();
    void cleanupShaders();
    
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
    void setDithering(bool enable) { enableDithering = enable; }
    void setFogRange(float start, float end) { fogStart = start; fogEnd = end; }
    void setFogColor(const Vec3& color) { fogColor = color; }
    void setLightDirection(const Vec3& dir) { lightDirection = dir; }
    void setLightColor(const Vec3& color) { lightColor = color; }
    
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
