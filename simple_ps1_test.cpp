#include "simple_ps1_renderer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

// Simple OBJ loader for testing
bool loadSimpleOBJ(const std::string& filename, std::vector<SimplePS1::PS1Vertex>& vertices, std::vector<unsigned int>& indices) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }
    
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        std::istringstream iss(line);
        std::string type;
        iss >> type;
        
        if (type == "v") {
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        } else if (type == "vt") {
            glm::vec2 tex;
            iss >> tex.x >> tex.y;
            texCoords.push_back(tex);
        } else if (type == "vn") {
            glm::vec3 norm;
            iss >> norm.x >> norm.y >> norm.z;
            normals.push_back(norm);
        } else if (type == "f") {
            std::string face;
            std::vector<std::string> faceParts;
            while (iss >> face) {
                faceParts.push_back(face);
            }
            
            // Simple triangulation for quads
            if (faceParts.size() >= 3) {
                for (size_t i = 1; i < faceParts.size() - 1; ++i) {
                    // Parse vertex indices
                    std::string v1 = faceParts[0];
                    std::string v2 = faceParts[i];
                    std::string v3 = faceParts[i + 1];
                    
                    // Extract position indices (ignore texture and normal for now)
                    size_t pos1 = std::stoi(v1.substr(0, v1.find('/'))) - 1;
                    size_t pos2 = std::stoi(v2.substr(0, v2.find('/'))) - 1;
                    size_t pos3 = std::stoi(v3.substr(0, v3.find('/'))) - 1;
                    
                    if (pos1 < positions.size() && pos2 < positions.size() && pos3 < positions.size()) {
                        // Create vertices
                        glm::vec3 p1 = positions[pos1];
                        glm::vec3 p2 = positions[pos2];
                        glm::vec3 p3 = positions[pos3];
                        
                        // Calculate normal
                        glm::vec3 normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));
                        
                        // Add vertices
                        unsigned int baseIndex = vertices.size();
                        vertices.push_back(SimplePS1::PS1Vertex(p1, glm::vec2(0, 0), normal, glm::vec3(0.8f, 0.6f, 0.4f)));
                        vertices.push_back(SimplePS1::PS1Vertex(p2, glm::vec2(1, 0), normal, glm::vec3(0.8f, 0.6f, 0.4f)));
                        vertices.push_back(SimplePS1::PS1Vertex(p3, glm::vec2(0, 1), normal, glm::vec3(0.8f, 0.6f, 0.4f)));
                        
                        // Add indices
                        indices.push_back(baseIndex);
                        indices.push_back(baseIndex + 1);
                        indices.push_back(baseIndex + 2);
                    }
                }
            }
        }
    }
    
    std::cout << "Loaded OBJ: " << vertices.size() << " vertices, " << indices.size() / 3 << " triangles" << std::endl;
    return true;
}

// Create a simple cube for testing
void createTestCube(std::vector<SimplePS1::PS1Vertex>& vertices, std::vector<unsigned int>& indices) {
    // Cube vertices
    std::vector<glm::vec3> positions = {
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
    
    std::vector<glm::vec3> normals = {
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
    
    std::vector<glm::vec3> colors = {
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
            glm::vec2(0.0f, 0.0f), // Simple texture coords
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
    std::cout << "=== Simple PS1 Graphics Test ===" << std::endl;
    
    // Create renderer
    SimplePS1::SimplePS1Renderer renderer(1024, 768, "PS1 Style Graphics Test");
    
    if (!renderer.initialize()) {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return -1;
    }
    
    // Create test meshes
    std::vector<SimplePS1::PS1Vertex> vertices;
    std::vector<unsigned int> indices;
    
    // Try to load OBJ files from assets
    bool loadedOBJ = false;
    if (loadSimpleOBJ("assets/converted/Forrest/trees/Tree1.obj", vertices, indices)) {
        renderer.addMesh(vertices, indices);
        loadedOBJ = true;
        std::cout << "Loaded Tree1.obj" << std::endl;
    }
    
    // If no OBJ loaded, create test cubes
    if (!loadedOBJ) {
        std::cout << "No OBJ files found, creating test cubes..." << std::endl;
        
        // Create multiple test cubes
        for (int x = -2; x <= 2; ++x) {
            for (int z = -2; z <= 2; ++z) {
                vertices.clear();
                indices.clear();
                createTestCube(vertices, indices);
                
                // Scale and position cubes
                for (auto& vertex : vertices) {
                    vertex.position *= 0.5f; // Scale down
                    vertex.position.x += x * 2.0f;
                    vertex.position.z += z * 2.0f;
                }
                
                renderer.addMesh(vertices, indices);
            }
        }
    }
    
    // Set up PS1-style settings
    renderer.setVertexLighting(true);
    renderer.setQuantizedLighting(true);
    renderer.setFog(true);
    renderer.setDithering(true);
    renderer.setFogRange(5.0f, 20.0f);
    renderer.setFogColor(glm::vec3(0.3f, 0.3f, 0.4f));
    renderer.setLightDirection(glm::vec3(0.5f, 1.0f, 0.3f));
    renderer.setLightColor(glm::vec3(1.0f, 0.9f, 0.8f));
    
    // Set camera position
    renderer.getCamera().setPosition(glm::vec3(0, 2, 5));
    
    std::cout << "Controls:" << std::endl;
    std::cout << "  WASD - Move" << std::endl;
    std::cout << "  Space/Shift - Up/Down" << std::endl;
    std::cout << "  Left Mouse - Look around" << std::endl;
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
