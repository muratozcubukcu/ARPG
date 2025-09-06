#pragma once

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace SimplePS1 {

struct PS1Vertex {
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
    glm::vec3 color; // For vertex colors
    
    PS1Vertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3& norm, const glm::vec3& col = glm::vec3(1.0f))
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
    glm::vec3 position;
    float yaw, pitch;
    float fov, nearPlane, farPlane;
    int screenWidth, screenHeight;
    
    PS1Camera(int width, int height);
    
    void update(float deltaTime);
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    
    // First-person controls
    void handleInput(GLFWwindow* window, float deltaTime);
    void setPosition(const glm::vec3& pos) { position = pos; }
    void addRotation(float yawDelta, float pitchDelta);
};

class SimplePS1Renderer {
private:
    GLFWwindow* window;
    PS1Camera camera;
    std::vector<std::unique_ptr<PS1Mesh>> meshes;
    
    // PS1-style settings
    bool enableVertexLighting = true;
    bool enableQuantizedLighting = true;
    bool enableFog = true;
    bool enableDithering = true;
    float fogStart = 10.0f;
    float fogEnd = 50.0f;
    glm::vec3 fogColor = glm::vec3(0.5f, 0.5f, 0.5f);
    
    // Lighting
    glm::vec3 lightDirection = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    
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
    void setFogColor(const glm::vec3& color) { fogColor = color; }
    void setLightDirection(const glm::vec3& dir) { lightDirection = dir; }
    void setLightColor(const glm::vec3& color) { lightColor = color; }
    
    // Camera access
    PS1Camera& getCamera() { return camera; }
    
    // Window access
    GLFWwindow* getWindow() { return window; }
    bool isOpen() const { return !glfwWindowShouldClose(window); }
    
    // Input handling
    void handleEvents();
    void handleInput(float deltaTime);
};

} // namespace SimplePS1
