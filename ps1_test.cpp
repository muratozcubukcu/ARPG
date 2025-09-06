#include "ps1_graphics_manager.h"
#include "ps1_mesh.h"
#include "ps1_material.h"
#include "ps1_camera.h"
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <cmath>

// Simple OBJ loader for testing
class SimpleOBJLoader {
public:
    struct Vertex {
        float x, y, z;
        float nx, ny, nz;
        float u, v;
    };
    
    static std::vector<Vertex> loadOBJ(const std::string& filename) {
        std::vector<Vertex> vertices;
        std::vector<Vec3> positions;
        std::vector<Vec3> normals;
        std::vector<Vec2> texCoords;
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open OBJ file: " << filename << std::endl;
            return vertices;
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
                // Simple face parsing (triangles only)
                std::string v1, v2, v3;
                iss >> v1 >> v2 >> v3;
                
                // Parse vertex indices
                int v1_idx = std::stoi(v1) - 1;
                int v2_idx = std::stoi(v2) - 1;
                int v3_idx = std::stoi(v3) - 1;
                
                // Add triangle vertices
                if (v1_idx < positions.size()) {
                    vertices.push_back({
                        positions[v1_idx].x, positions[v1_idx].y, positions[v1_idx].z,
                        0.0f, 1.0f, 0.0f, // Default normal
                        0.0f, 0.0f // Default UV
                    });
                }
                if (v2_idx < positions.size()) {
                    vertices.push_back({
                        positions[v2_idx].x, positions[v2_idx].y, positions[v2_idx].z,
                        0.0f, 1.0f, 0.0f, // Default normal
                        0.0f, 0.0f // Default UV
                    });
                }
                if (v3_idx < positions.size()) {
                    vertices.push_back({
                        positions[v3_idx].x, positions[v3_idx].y, positions[v3_idx].z,
                        0.0f, 1.0f, 0.0f, // Default normal
                        0.0f, 0.0f // Default UV
                    });
                }
            }
        }
        
        file.close();
        std::cout << "Loaded OBJ: " << filename << " (" << vertices.size() << " vertices)" << std::endl;
        return vertices;
    }
};

// Simple input handler for testing
class TestInputHandler {
private:
    bool keys[256] = {false};
    float mouseX = 0.0f, mouseY = 0.0f;
    float lastMouseX = 0.0f, lastMouseY = 0.0f;
    bool mouseCaptured = false;
    
public:
    void update() {
        // Simple keyboard input simulation for testing
        // In a real implementation, this would use your input system
        
        // Simulate WASD movement
        if (keys['W']) {
            // Move forward
        }
        if (keys['S']) {
            // Move backward
        }
        if (keys['A']) {
            // Move left
        }
        if (keys['D']) {
            // Move right
        }
        
        // Simulate mouse look
        float deltaX = mouseX - lastMouseX;
        float deltaY = mouseY - lastMouseY;
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        
        if (mouseCaptured && (deltaX != 0.0f || deltaY != 0.0f)) {
            // Handle mouse look
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
};

// Test program for PS1 graphics with movement and look-around
int main() {
    std::cout << "=== PS1 Graphics Test with Movement ===" << std::endl;
    
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
    
    // Load test assets
    std::cout << "Loading test assets..." << std::endl;
    
    // Load tree models
    auto tree1Vertices = SimpleOBJLoader::loadOBJ("assets/converted/Forrest/trees/Tree1.obj");
    auto tree2Vertices = SimpleOBJLoader::loadOBJ("assets/converted/Forrest/trees/Tree2.obj");
    auto rock1Vertices = SimpleOBJLoader::loadOBJ("assets/converted/Forrest/rocks/Rock1.obj");
    
    // Create PS1 meshes from loaded data
    std::vector<std::shared_ptr<VKFW::Core::PS1Mesh>> meshes;
    
    // Create tree meshes
    if (!tree1Vertices.empty()) {
        auto tree1Mesh = std::make_shared<VKFW::Core::PS1Mesh>("Tree1");
        tree1Mesh->setup_ps1_style();
        
        // Configure for low-poly style
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
    
    if (!tree2Vertices.empty()) {
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
    
    if (!rock1Vertices.empty()) {
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
    VKFW::Vec3 startPos(0.0f, 0.0f, 5.0f);
    float startYaw = 0.0f;
    float startPitch = 0.0f;
    camera->update_first_person(startYaw, startPitch, startPos);
    
    // Input handler
    TestInputHandler inputHandler;
    
    // Movement variables
    VKFW::Vec3 position = startPos;
    float yaw = startYaw;
    float pitch = startPitch;
    float moveSpeed = 5.0f;
    float mouseSensitivity = 0.1f;
    
    // Render loop
    std::cout << "Starting PS1-style rendering loop with movement..." << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  WASD - Move around" << std::endl;
    std::cout << "  Mouse - Look around" << std::endl;
    std::cout << "  ESC - Exit" << std::endl;
    
    auto lastTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;
    
    while (frameCount < 1000) { // Run for ~16 seconds at 60 FPS
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        // Simulate input for testing
        // In a real implementation, this would come from your input system
        
        // Simulate movement based on time
        float time = frameCount * 0.016f; // 60 FPS
        
        // Circular movement for testing
        float radius = 10.0f;
        position.x = std::cos(time * 0.5f) * radius;
        position.z = std::sin(time * 0.5f) * radius;
        position.y = 2.0f + std::sin(time * 0.3f) * 1.0f; // Slight vertical movement
        
        // Look around
        yaw = time * 30.0f; // Slow rotation
        pitch = std::sin(time * 0.2f) * 10.0f; // Gentle pitch movement
        
        // Update camera
        camera->update_first_person(yaw, pitch, position);
        
        // Update input
        inputHandler.update();
        
        // Render frame
        graphicsManager->render_frame();
        
        // Print status every 60 frames
        if (frameCount % 60 == 0) {
            std::cout << "Frame " << frameCount 
                      << " - Position: (" << position.x << ", " << position.y << ", " << position.z << ")"
                      << " - Yaw: " << yaw << " - Pitch: " << pitch << std::endl;
        }
        
        frameCount++;
        
        // Frame rate limiting
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
    
    std::cout << "PS1 Graphics Test completed" << std::endl;
    std::cout << "Total frames rendered: " << frameCount << std::endl;
    
    // Cleanup
    graphicsManager->shutdown();
    
    return 0;
}
