#include "simple_ps1_basic.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

// Create a simple cube for testing
void createTestCube(std::vector<SimplePS1::PS1Vertex>& vertices, std::vector<unsigned int>& indices) {
    // Cube vertices
    std::vector<SimplePS1::Vec3> positions = {
        // Front face
        {-1.0f, -1.0f,  1.0f}, { 1.0f, -1.0f,  1.0f}, { 1.0f,  1.0f,  1.0f}, {-1.0f,  1.0f,  1.0f},
        // Back face
        {-1.0f, -1.0f, -1.0f}, {-1.0f,  1.0f, -1.0f}, { 1.0f,  1.0f, -1.0f}, { 1.0f, -1.0f, -1.0f},
        // Top face
        {-1.0f,  1.0f, -1.0f}, {-1.0f,  1.0f,  1.0f}, { 1.0f,  1.0f,  1.0f}, { 1.0f,  1.0f, -1.0f},
        // Bottom face
        {-1.0f, -1.0f, -1.0f}, { 1.0f, -1.0f, -1.0f}, { 1.0f, -1.0f,  1.0f}, {-1.0f, -1.0f,  1.0f},
        // Right face
        { 1.0f, -1.0f, -1.0f}, { 1.0f,  1.0f, -1.0f}, { 1.0f,  1.0f,  1.0f}, { 1.0f, -1.0f,  1.0f},
        // Left face
        {-1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f,  1.0f}, {-1.0f,  1.0f,  1.0f}, {-1.0f,  1.0f, -1.0f}
    };
    
    std::vector<SimplePS1::Vec3> normals = {
        // Front face
        { 0.0f,  0.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}, { 0.0f,  0.0f,  1.0f},
        // Back face
        { 0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f, -1.0f},
        // Top face
        { 0.0f,  1.0f,  0.0f}, { 0.0f,  1.0f,  0.0f}, { 0.0f,  1.0f,  0.0f}, { 0.0f,  1.0f,  0.0f},
        // Bottom face
        { 0.0f, -1.0f,  0.0f}, { 0.0f, -1.0f,  0.0f}, { 0.0f, -1.0f,  0.0f}, { 0.0f, -1.0f,  0.0f},
        // Right face
        { 1.0f,  0.0f,  0.0f}, { 1.0f,  0.0f,  0.0f}, { 1.0f,  0.0f,  0.0f}, { 1.0f,  0.0f,  0.0f},
        // Left face
        {-1.0f,  0.0f,  0.0f}, {-1.0f,  0.0f,  0.0f}, {-1.0f,  0.0f,  0.0f}, {-1.0f,  0.0f,  0.0f}
    };
    
    std::vector<SimplePS1::Vec3> colors = {
        {0.8f, 0.2f, 0.2f}, {0.2f, 0.8f, 0.2f}, {0.2f, 0.2f, 0.8f}, {0.8f, 0.8f, 0.2f},
        {0.8f, 0.2f, 0.8f}, {0.2f, 0.8f, 0.8f}, {0.5f, 0.5f, 0.5f}, {0.9f, 0.9f, 0.9f},
        {0.3f, 0.7f, 0.3f}, {0.7f, 0.3f, 0.7f}, {0.3f, 0.3f, 0.7f}, {0.7f, 0.7f, 0.3f},
        {0.4f, 0.6f, 0.4f}, {0.6f, 0.4f, 0.6f}, {0.4f, 0.4f, 0.6f}, {0.6f, 0.6f, 0.4f},
        {0.1f, 0.9f, 0.1f}, {0.9f, 0.1f, 0.9f}, {0.1f, 0.1f, 0.9f}, {0.9f, 0.9f, 0.1f},
        {0.2f, 0.8f, 0.2f}, {0.8f, 0.2f, 0.8f}, {0.2f, 0.2f, 0.8f}, {0.8f, 0.8f, 0.2f}
    };
    
    // Create vertices
    for (size_t i = 0; i < positions.size(); ++i) {
        vertices.push_back(SimplePS1::PS1Vertex(
            positions[i],
            SimplePS1::Vec2(0.0f, 0.0f), // Simple texture coords
            normals[i],
            colors[i]
        ));
    }
    
    // Create indices for triangles
    std::vector<unsigned int> cubeIndices = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Top face
        8, 9, 10, 10, 11, 8,
        // Bottom face
        12, 13, 14, 14, 15, 12,
        // Right face
        16, 17, 18, 18, 19, 16,
        // Left face
        20, 21, 22, 22, 23, 20
    };
    
    indices = cubeIndices;
}

int main() {
    std::cout << "=== Simple PS1 Graphics Test (Basic OpenGL) ===" << std::endl;
    
    // Create renderer
    SimplePS1::SimplePS1Renderer renderer(1024, 768, "PS1 Style Graphics Test - Basic");
    
    if (!renderer.initialize()) {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return -1;
    }
    
    // Create test meshes
    std::vector<SimplePS1::PS1Vertex> vertices;
    std::vector<unsigned int> indices;
    
    // Create multiple test cubes
    std::cout << "Creating test cubes..." << std::endl;
    for (int x = -2; x <= 2; ++x) {
        for (int z = -2; z <= 2; ++z) {
            vertices.clear();
            indices.clear();
            createTestCube(vertices, indices);
            
            // Scale and position cubes
            for (auto& vertex : vertices) {
                vertex.position = vertex.position * 0.5f; // Scale down
                vertex.position = vertex.position + SimplePS1::Vec3(x * 2.0f, 0, z * 2.0f);
            }
            
            renderer.addMesh(vertices, indices);
        }
    }
    
    // Set up PS1-style settings
    renderer.setVertexLighting(true);
    renderer.setQuantizedLighting(true);
    renderer.setFog(true);
    renderer.setFogColor(SimplePS1::Vec3(0.3f, 0.3f, 0.4f));
    
    // Set camera position
    renderer.getCamera().setPosition(SimplePS1::Vec3(0, 2, 10));
    
    std::cout << "Controls:" << std::endl;
    std::cout << "  WASD - Move (relative to camera direction)" << std::endl;
    std::cout << "  Space/Shift - Up/Down" << std::endl;
    std::cout << "  Right Mouse - Look around (hold and move mouse)" << std::endl;
    std::cout << "  ESC - Exit" << std::endl;
    
    // Main loop
    sf::Clock clock;
    int frameCount = 0;
    
    while (renderer.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        frameCount++;
        
        // Handle events
        renderer.handleEvents();
        
        // Handle input
        renderer.handleInput(deltaTime);
        
        // Update camera
        renderer.getCamera().update(deltaTime);
        
        // Render frame
        renderer.beginFrame();
        renderer.renderFrame();
        renderer.endFrame();
        
        // Print FPS every 60 frames
        if (frameCount % 60 == 0) {
            float fps = 1.0f / deltaTime;
            std::cout << "FPS: " << fps << " - Position: (" 
                      << renderer.getCamera().position.x << ", "
                      << renderer.getCamera().position.y << ", "
                      << renderer.getCamera().position.z << ")" << std::endl;
        }
        
        // Check for exit
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            break;
        }
    }
    
    std::cout << "Test completed!" << std::endl;
    return 0;
}
