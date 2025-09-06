#include "simple_ps1_basic.h"
#include <iostream>

namespace SimplePS1 {

SimplePS1Renderer::SimplePS1Renderer(int width, int height, const std::string& title)
    : window(sf::VideoMode(width, height), title, sf::Style::Default, sf::ContextSettings(24, 8, 4, 3, 3)),
      camera(width, height) {
    
    // Enable VSync
    window.setVerticalSyncEnabled(true);
    
    // Set up OpenGL
    setupOpenGL();
}

SimplePS1Renderer::~SimplePS1Renderer() {
    shutdown();
}

bool SimplePS1Renderer::initialize() {
    return true;
}

void SimplePS1Renderer::shutdown() {
    clearMeshes();
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
    
    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    // Set up light
    float lightPos[] = {0.5f, 1.0f, 0.3f, 0.0f};
    float lightColor[] = {1.0f, 0.9f, 0.8f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    
    // Enable fog
    glEnable(GL_FOG);
    glFogf(GL_FOG_START, 10.0f);
    glFogf(GL_FOG_END, 50.0f);
    glFogf(GL_FOG_MODE, GL_LINEAR);
    float fogColor[] = {0.5f, 0.5f, 0.5f, 1.0f};
    glFogfv(GL_FOG_COLOR, fogColor);
}

PS1Mesh* SimplePS1Renderer::addMesh(const std::vector<PS1Vertex>& vertices, const std::vector<unsigned int>& indices) {
    auto mesh = std::make_unique<PS1Mesh>();
    mesh->vertices = vertices;
    mesh->indices = indices;
    
    PS1Mesh* ptr = mesh.get();
    meshes.push_back(std::move(mesh));
    return ptr;
}

void SimplePS1Renderer::clearMeshes() {
    meshes.clear();
}

void SimplePS1Renderer::beginFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Set up projection
    camera.setupProjection();
    
    // Set up view
    camera.setupView();
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
