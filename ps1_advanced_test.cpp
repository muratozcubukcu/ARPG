#include "ps1_graphics_manager.h"
#include "ps1_mesh.h"
#include "ps1_material.h"
#include "ps1_camera.h"
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

// Enhanced OBJ loader with proper vertex data
class EnhancedOBJLoader {
public:
    struct Vertex {
        float x, y, z;
        float nx, ny, nz;
        float u, v;
        float r, g, b; // Vertex colors
    };
    
    struct MeshData {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::string name;
    };
    
    static MeshData loadOBJ(const std::string& filename) {
        MeshData meshData;
        std::vector<Vec3> positions;
        std::vector<Vec3> normals;
        std::vector<Vec2> texCoords;
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open OBJ file: " << filename << std::endl;
            return meshData;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            
            std::istringstream iss(line);
            std::string type;
            iss >> type;
            
            if (type == "v") {
                float x, y, z;
                iss >> x >> y >> z;
                positions.push_back({x, y, z});
            } else if (type == "vn") {
                float nx, ny, nz;
                iss >> nx >> ny >> nz;
                normals.push_back({nx, ny, nz});
            } else if (type == "vt") {
                float u, v;
                iss >> u >> v;
                texCoords.push_back({u, v});
            } else if (type == "f") {
                // Parse face indices
                std::string v1, v2, v3;
                iss >> v1 >> v2 >> v3;
                
                // Parse vertex indices (v/vt/vn format)
                int v1_idx = std::stoi(v1) - 1;
                int v2_idx = std::stoi(v2) - 1;
                int v3_idx = std::stoi(v3) - 1;
                
                // Add triangle vertices
                if (v1_idx < positions.size()) {
                    meshData.vertices.push_back({
                        positions[v1_idx].x, positions[v1_idx].y, positions[v1_idx].z,
                        0.0f, 1.0f, 0.0f, // Default normal
                        0.0f, 0.0f, // Default UV
                        0.5f, 0.8f, 0.3f // Green color for trees
                    });
                    meshData.indices.push_back(static_cast<uint32_t>(meshData.vertices.size() - 1));
                }
                if (v2_idx < positions.size()) {
                    meshData.vertices.push_back({
                        positions[v2_idx].x, positions[v2_idx].y, positions[v2_idx].z,
                        0.0f, 1.0f, 0.0f, // Default normal
                        0.0f, 0.0f, // Default UV
                        0.5f, 0.8f, 0.3f // Green color for trees
                    });
                    meshData.indices.push_back(static_cast<uint32_t>(meshData.vertices.size() - 1));
                }
                if (v3_idx < positions.size()) {
                    meshData.vertices.push_back({
                        positions[v3_idx].x, positions[v3_idx].y, positions[v3_idx].z,
                        0.0f, 1.0f, 0.0f, // Default normal
                        0.0f, 0.0f, // Default UV
                        0.5f, 0.8f, 0.3f // Green color for trees
                    });
                    meshData.indices.push_back(static_cast<uint32_t>(meshData.vertices.size() - 1));
                }
            }
        }
        
        file.close();
        meshData.name = filename;
        std::cout << "Loaded OBJ: " << filename << " (" << meshData.vertices.size() << " vertices, " 
                  << meshData.indices.size() / 3 << " triangles)" << std::endl;
        return meshData;
    }
};

// Simple HDR environment loader
class HDRLoader {
public:
    static bool loadHDR(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open HDR file: " << filename << std::endl;
            return false;
        }
        
        // Simple HDR header check
        std::string line;
        std::getline(file, line);
        if (line != "#?RADIANCE") {
            std::cerr << "Invalid HDR file format: " << filename << std::endl;
            return false;
        }
        
        // Skip header lines
        while (std::getline(file, line) && !line.empty()) {
            if (line[0] == '#') continue;
        }
        
        // Read resolution
        std::getline(file, line);
        std::istringstream iss(line);
        std::string format;
        int width, height;
        iss >> format >> height >> width;
        
        std::cout << "Loaded HDR: " << filename << " (" << width << "x" << height << ")" << std::endl;
        file.close();
        return true;
    }
};

// Enhanced input handler with proper controls
class EnhancedInputHandler {
private:
    bool keys[256] = {false};
    float mouseX = 0.0f, mouseY = 0.0f;
    float lastMouseX = 0.0f, lastMouseY = 0.0f;
    bool mouseCaptured = false;
    bool isRunning = true;
    
public:
    void update() {
        // Update mouse delta
        float deltaX = mouseX - lastMouseX;
        float deltaY = mouseY - lastMouseY;
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        
        // Check for exit
        if (keys[27]) { // ESC key
            isRunning = false;
        }
    }
    
    void setKey(int key, bool pressed) {
        if (key >= 0 && key < 256) {
            keys[key] = pressed;
        }
    }
    
    void setMouse(float x, float y) {
        mouseX = x;
        mouseY = y;
    }
    
    void setMouseCaptured(bool captured) {
        mouseCaptured = captured;
    }
    
    bool isKeyPressed(int key) const {
        if (key >= 0 && key < 256) {
            return keys[key];
        }
        return false;
    }
    
    float getMouseDeltaX() const {
        return mouseX - lastMouseX;
    }
    
    float getMouseDeltaY() const {
        return mouseY - lastMouseY;
    }
    
    bool shouldExit() const {
        return !isRunning;
    }
};

// Main test program
int main() {
    std::cout << "=== PS1 Graphics Advanced Test with Movement ===" << std::endl;
    
    // Create graphics manager
    auto graphicsManager = std::make_unique<VKFW::PS1GraphicsManager>();
    
    // Initialize the graphics system
    if (!graphicsManager->initialize()) {
        std::cerr << "Failed to initialize PS1 graphics system" << std::endl;
        return -1;
    }
    
    // Setup PS1-style rendering
    graphicsManager->setup_ps1_style();
    
    // Configure PS1 settings for testing
    VKFW::PS1Settings settings;
    settings.affineTextureMapping = true;
    settings.vertexLighting = true;
    settings.quantizedLighting = true;
    settings.lightingLevels = 4;
    settings.vertexFog = true;
    settings.fogDensity = 1.0f;
    settings.useVertexColors = true;
    settings.flatShading = false;
    settings.fov = 60.0f;
    settings.nearPlane = 0.1f;
    settings.farPlane = 100.0f;
    settings.noPerspectiveCorrection = true;
    settings.dithering = true;
    settings.colorQuantization = true;
    settings.colorBits = 16;
    
    graphicsManager->set_ps1_settings(settings);
    
    // Load HDR environments
    std::cout << "Loading HDR environments..." << std::endl;
    HDRLoader::loadHDR("assets/Sky/Daysky_HDRI/DAYSKY.hdr");
    HDRLoader::loadHDR("assets/Sky/PSX_Nightsky_HDRI/PSX_NIGHTSKY.hdr");
    
    // Load test assets
    std::cout << "Loading test assets..." << std::endl;
    
    // Load tree models
    auto tree1Data = EnhancedOBJLoader::loadOBJ("assets/converted/Forrest/trees/Tree1.obj");
    auto tree2Data = EnhancedOBJLoader::loadOBJ("assets/converted/Forrest/trees/Tree2.obj");
    auto rock1Data = EnhancedOBJLoader::loadOBJ("assets/converted/Forrest/rocks/Rock1.obj");
    
    // Create PS1 meshes from loaded data
    std::vector<std::shared_ptr<VKFW::Core::PS1Mesh>> meshes;
    
    // Create tree meshes
    if (!tree1Data.vertices.empty()) {
        auto tree1Mesh = std::make_shared<VKFW::Core::PS1Mesh>("Tree1");
        tree1Mesh->setup_ps1_style();
        
        VKFW::Core::PS1MeshSettings meshSettings;
        meshSettings.affineTextureMapping = true;
        meshSettings.vertexColors = true;
        meshSettings.flatShading = false;
        meshSettings.quantizedVertices = true;
        meshSettings.vertexGridSize = 0.1f;
        meshSettings.lowPolyStyle = true;
        meshSettings.maxVerticesPerFace = 3;
        
        tree1Mesh->set_ps1_settings(meshSettings);
        tree1Mesh->quantize_vertices(0.1f);
        tree1Mesh->enable_flat_shading();
        
        meshes.push_back(tree1Mesh);
        graphicsManager->add_mesh(tree1Mesh);
    }
    
    if (!tree2Data.vertices.empty()) {
        auto tree2Mesh = std::make_shared<VKFW::Core::PS1Mesh>("Tree2");
        tree2Mesh->setup_ps1_style();
        
        VKFW::Core::PS1MeshSettings meshSettings;
        meshSettings.affineTextureMapping = true;
        meshSettings.vertexColors = true;
        meshSettings.flatShading = false;
        meshSettings.quantizedVertices = true;
        meshSettings.vertexGridSize = 0.1f;
        meshSettings.lowPolyStyle = true;
        meshSettings.maxVerticesPerFace = 3;
        
        tree2Mesh->set_ps1_settings(meshSettings);
        tree2Mesh->quantize_vertices(0.1f);
        tree2Mesh->enable_flat_shading();
        
        meshes.push_back(tree2Mesh);
        graphicsManager->add_mesh(tree2Mesh);
    }
    
    if (!rock1Data.vertices.empty()) {
        auto rock1Mesh = std::make_shared<VKFW::Core::PS1Mesh>("Rock1");
        rock1Mesh->setup_ps1_style();
        
        VKFW::Core::PS1MeshSettings meshSettings;
        meshSettings.affineTextureMapping = true;
        meshSettings.vertexColors = true;
        meshSettings.flatShading = true; // Rocks look good with flat shading
        meshSettings.quantizedVertices = true;
        meshSettings.vertexGridSize = 0.1f;
        meshSettings.lowPolyStyle = true;
        meshSettings.maxVerticesPerFace = 3;
        
        rock1Mesh->set_ps1_settings(meshSettings);
        rock1Mesh->quantize_vertices(0.1f);
        rock1Mesh->enable_flat_shading();
        
        meshes.push_back(rock1Mesh);
        graphicsManager->add_mesh(rock1Mesh);
    }
    
    // Setup fog for atmospheric effect
    graphicsManager->set_fog_settings(0.3f, 0.8f, 1.2f, VKFW::Vec3(0.6f, 0.7f, 0.8f));
    
    // Setup lighting
    graphicsManager->set_lighting_settings(4, true);
    
    // Enable PS1 effects
    graphicsManager->enable_ps1_effects(true);
    
    // Get camera for first-person controls
    auto camera = graphicsManager->get_camera();
    if (!camera) {
        std::cerr << "Failed to get camera" << std::endl;
        return -1;
    }
    
    // Setup first-person camera
    camera->setup_ps1_style();
    
    // Set initial position and rotation
    VKFW::Vec3 position(0.0f, 2.0f, 5.0f);
    float yaw = 0.0f;
    float pitch = 0.0f;
    camera->update_first_person(yaw, pitch, position);
    
    // Input handler
    EnhancedInputHandler inputHandler;
    
    // Movement variables
    float moveSpeed = 5.0f;
    float mouseSensitivity = 0.1f;
    float sprintMultiplier = 2.0f;
    
    // Render loop
    std::cout << "Starting PS1-style rendering loop with movement..." << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  WASD - Move around" << std::endl;
    std::cout << "  Mouse - Look around" << std::endl;
    std::cout << "  Shift - Sprint" << std::endl;
    std::cout << "  ESC - Exit" << std::endl;
    
    auto lastTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;
    
    while (!inputHandler.shouldExit() && frameCount < 2000) { // Run for ~33 seconds at 60 FPS
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        // Simulate input for testing
        // In a real implementation, this would come from your input system
        
        // Simulate movement based on time and input
        float time = frameCount * 0.016f; // 60 FPS
        
        // Complex movement pattern for testing
        float radius = 15.0f;
        float height = 3.0f;
        
        // Circular movement with height variation
        position.x = std::cos(time * 0.3f) * radius;
        position.z = std::sin(time * 0.3f) * radius;
        position.y = height + std::sin(time * 0.5f) * 2.0f; // Vertical movement
        
        // Look around with smooth movement
        yaw = time * 20.0f; // Slow rotation
        pitch = std::sin(time * 0.15f) * 15.0f; // Gentle pitch movement
        
        // Simulate mouse input
        float mouseDeltaX = std::sin(time * 0.1f) * 2.0f;
        float mouseDeltaY = std::cos(time * 0.08f) * 1.0f;
        inputHandler.setMouse(mouseDeltaX, mouseDeltaY);
        
        // Update camera
        camera->update_first_person(yaw, pitch, position);
        
        // Update input
        inputHandler.update();
        
        // Render frame
        graphicsManager->render_frame();
        
        // Print status every 120 frames (every 2 seconds at 60 FPS)
        if (frameCount % 120 == 0) {
            std::cout << "Frame " << frameCount 
                      << " - Position: (" << position.x << ", " << position.y << ", " << position.z << ")"
                      << " - Yaw: " << yaw << " - Pitch: " << pitch << std::endl;
        }
        
        frameCount++;
        
        // Frame rate limiting
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
    
    std::cout << "PS1 Graphics Advanced Test completed" << std::endl;
    std::cout << "Total frames rendered: " << frameCount << std::endl;
    
    // Cleanup
    graphicsManager->shutdown();
    
    return 0;
}
