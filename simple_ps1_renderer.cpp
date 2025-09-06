#include "simple_ps1_renderer.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace SimplePS1 {

// Vertex shader source
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform bool enableVertexLighting;
uniform bool enableQuantizedLighting;

out vec2 TexCoord;
out vec3 Normal;
out vec3 Color;
out float LightIntensity;
out float FogFactor;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    Normal = aNormal;
    Color = aColor;
    
    // PS1-style vertex lighting
    if (enableVertexLighting) {
        float lightIntensity = max(dot(normalize(aNormal), normalize(lightDir)), 0.0);
        
        // PS1-style quantized lighting
        if (enableQuantizedLighting) {
            lightIntensity = floor(lightIntensity * 4.0) / 4.0; // Quantize to 4 levels
        }
        
        LightIntensity = lightIntensity;
    } else {
        LightIntensity = 1.0;
    }
    
    // PS1-style vertex fog
    FogFactor = clamp((gl_Position.z - 0.5) / 0.5, 0.0, 1.0);
}
)";

// Fragment shader source
const char* fragmentShaderSource = R"(
#version 330 core
in vec2 TexCoord;
in vec3 Normal;
in vec3 Color;
in float LightIntensity;
in float FogFactor;

uniform vec3 lightColor;
uniform bool enableFog;
uniform bool enableDithering;
uniform vec3 fogColor;

out vec4 FragColor;

// Simple dithering pattern
float dither(vec2 coord) {
    vec2 c = floor(coord * 4.0) / 4.0;
    return fract(sin(dot(c, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    // Base color from vertex color
    vec3 baseColor = Color;
    
    // Apply lighting
    vec3 litColor = baseColor * LightIntensity * lightColor;
    
    // PS1-style dithering
    if (enableDithering) {
        float ditherValue = dither(gl_FragCoord.xy);
        litColor = floor(litColor * 4.0 + ditherValue) / 4.0;
    }
    
    // PS1-style fog
    if (enableFog) {
        litColor = mix(litColor, fogColor, FogFactor);
    }
    
    FragColor = vec4(litColor, 1.0);
}
)";

PS1Mesh::PS1Mesh() : vao(0), vbo(0), ebo(0) {}

PS1Mesh::~PS1Mesh() {
    cleanup();
}

void PS1Mesh::upload() {
    if (uploaded) return;
    
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    
    glBindVertexArray(vao);
    
    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PS1Vertex), vertices.data(), GL_STATIC_DRAW);
    
    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    // Set vertex attributes
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PS1Vertex), (void*)offsetof(PS1Vertex, position));
    glEnableVertexAttribArray(0);
    
    // Texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(PS1Vertex), (void*)offsetof(PS1Vertex, texCoord));
    glEnableVertexAttribArray(1);
    
    // Normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(PS1Vertex), (void*)offsetof(PS1Vertex, normal));
    glEnableVertexAttribArray(2);
    
    // Color
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(PS1Vertex), (void*)offsetof(PS1Vertex, color));
    glEnableVertexAttribArray(3);
    
    glBindVertexArray(0);
    uploaded = true;
}

void PS1Mesh::render() {
    if (!uploaded) upload();
    
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void PS1Mesh::cleanup() {
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (ebo) glDeleteBuffers(1, &ebo);
    vao = vbo = ebo = 0;
    uploaded = false;
}

PS1Camera::PS1Camera(int width, int height) 
    : position(0, 0, 0), yaw(0), pitch(0), fov(60.0f), nearPlane(0.1f), farPlane(100.0f),
      screenWidth(width), screenHeight(height) {}

void PS1Camera::update(float deltaTime) {
    // Camera update logic can be added here
}

glm::mat4 PS1Camera::getViewMatrix() const {
    glm::mat4 view = glm::mat4(1.0f);
    
    // Apply pitch rotation (around X axis)
    view = glm::rotate(view, glm::radians(-pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    
    // Apply yaw rotation (around Y axis)
    view = glm::rotate(view, glm::radians(-yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    
    // Apply translation
    view = glm::translate(view, -position);
    
    return view;
}

glm::mat4 PS1Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(fov), (float)screenWidth / (float)screenHeight, nearPlane, farPlane);
}

void PS1Camera::handleInput(const sf::Window& window, float deltaTime) {
    const float moveSpeed = 5.0f;
    const float mouseSensitivity = 0.1f;
    
    // Keyboard movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        position += glm::vec3(0, 0, -moveSpeed * deltaTime);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        position += glm::vec3(0, 0, moveSpeed * deltaTime);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        position += glm::vec3(-moveSpeed * deltaTime, 0, 0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        position += glm::vec3(moveSpeed * deltaTime, 0, 0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        position += glm::vec3(0, moveSpeed * deltaTime, 0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
        position += glm::vec3(0, -moveSpeed * deltaTime, 0);
    }
    
    // Mouse look
    static bool mouseCaptured = false;
    static sf::Vector2i lastMousePos;
    
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (!mouseCaptured) {
            mouseCaptured = true;
            lastMousePos = sf::Mouse::getPosition(window);
        } else {
            sf::Vector2i currentMousePos = sf::Mouse::getPosition(window);
            sf::Vector2i delta = currentMousePos - lastMousePos;
            
            yaw += delta.x * mouseSensitivity;
            pitch += delta.y * mouseSensitivity;
            
            // Clamp pitch
            pitch = glm::clamp(pitch, -89.0f, 89.0f);
            
            lastMousePos = currentMousePos;
        }
    } else {
        mouseCaptured = false;
    }
}

void PS1Camera::addRotation(float yawDelta, float pitchDelta) {
    yaw += yawDelta;
    pitch += pitchDelta;
    pitch = glm::clamp(pitch, -89.0f, 89.0f);
}

SimplePS1Renderer::SimplePS1Renderer(int width, int height, const std::string& title)
    : window(sf::VideoMode(width, height), title, sf::Style::Default, sf::ContextSettings(24, 8, 4, 3, 3)),
      camera(width, height), shaderProgram(0), vertexShader(0), fragmentShader(0) {
    
    // Enable VSync
    window.setVerticalSyncEnabled(true);
    
    // Set up OpenGL
    setupOpenGL();
}

SimplePS1Renderer::~SimplePS1Renderer() {
    cleanupShaders();
}

bool SimplePS1Renderer::initialize() {
    if (!setupShaders()) {
        std::cerr << "Failed to setup shaders!" << std::endl;
        return false;
    }
    
    // Get uniform locations
    uModel = glGetUniformLocation(shaderProgram, "model");
    uView = glGetUniformLocation(shaderProgram, "view");
    uProjection = glGetUniformLocation(shaderProgram, "projection");
    uLightDir = glGetUniformLocation(shaderProgram, "lightDir");
    uLightColor = glGetUniformLocation(shaderProgram, "lightColor");
    uFogStart = glGetUniformLocation(shaderProgram, "fogStart");
    uFogEnd = glGetUniformLocation(shaderProgram, "fogEnd");
    uFogColor = glGetUniformLocation(shaderProgram, "fogColor");
    uEnableVertexLighting = glGetUniformLocation(shaderProgram, "enableVertexLighting");
    uEnableQuantizedLighting = glGetUniformLocation(shaderProgram, "enableQuantizedLighting");
    uEnableFog = glGetUniformLocation(shaderProgram, "enableFog");
    uEnableDithering = glGetUniformLocation(shaderProgram, "enableDithering");
    
    return true;
}

void SimplePS1Renderer::shutdown() {
    clearMeshes();
    cleanupShaders();
}

void SimplePS1Renderer::setupOpenGL() {
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    // Enable face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // Set clear color (dark gray for PS1 feel)
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

bool SimplePS1Renderer::setupShaders() {
    // Create vertex shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    
    // Check vertex shader compilation
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
        return false;
    }
    
    // Create fragment shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    
    // Check fragment shader compilation
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed: " << infoLog << std::endl;
        return false;
    }
    
    // Create shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // Check linking
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        return false;
    }
    
    // Clean up individual shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return true;
}

void SimplePS1Renderer::cleanupShaders() {
    if (shaderProgram) {
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
}

PS1Mesh* SimplePS1Renderer::addMesh(const std::vector<PS1Vertex>& vertices, const std::vector<unsigned int>& indices) {
    auto mesh = std::make_unique<PS1Mesh>();
    mesh->vertices = vertices;
    mesh->indices = indices;
    mesh->upload();
    
    PS1Mesh* ptr = mesh.get();
    meshes.push_back(std::move(mesh));
    return ptr;
}

void SimplePS1Renderer::clearMeshes() {
    meshes.clear();
}

void SimplePS1Renderer::beginFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);
    
    // Set uniforms
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    
    glUniformMatrix4fv(uView, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(uModel, 1, GL_FALSE, &model[0][0]);
    
    glUniform3fv(uLightDir, 1, &lightDirection[0]);
    glUniform3fv(uLightColor, 1, &lightColor[0]);
    glUniform3fv(uFogColor, 1, &fogColor[0]);
    glUniform1i(uEnableVertexLighting, enableVertexLighting ? 1 : 0);
    glUniform1i(uEnableQuantizedLighting, enableQuantizedLighting ? 1 : 0);
    glUniform1i(uEnableFog, enableFog ? 1 : 0);
    glUniform1i(uEnableDithering, enableDithering ? 1 : 0);
}

void SimplePS1Renderer::renderFrame() {
    for (auto& mesh : meshes) {
        mesh->render();
    }
}

void SimplePS1Renderer::endFrame() {
    window.display();
}

void SimplePS1Renderer::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
    }
}

void SimplePS1Renderer::handleInput(float deltaTime) {
    camera.handleInput(window, deltaTime);
}

} // namespace SimplePS1
