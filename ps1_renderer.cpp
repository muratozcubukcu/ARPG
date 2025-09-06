#include "ps1_renderer.h"
#include "graphics/Vulkan-Engine/src/graphics/pipeline.h"
#include "graphics/Vulkan-Engine/src/graphics/descriptors.h"
#include "graphics/Vulkan-Engine/src/graphics/command_buffer.h"
#include <algorithm>
#include <cmath>

namespace VKFW {
namespace Systems {

PS1StyleRenderer::PS1StyleRenderer() : ForwardRenderer() {
    setup_ps1_style();
}

void PS1StyleRenderer::setup_ps1_style() {
    // Configure PS1-style settings
    m_ps1Settings.affineTextureMapping = true;
    m_ps1Settings.vertexLighting = true;
    m_ps1Settings.quantizedLighting = true;
    m_ps1Settings.lightingLevels = 4;
    m_ps1Settings.vertexFog = true;
    m_ps1Settings.fogDensity = 1.0f;
    m_ps1Settings.useVertexColors = true;
    m_ps1Settings.flatShading = false;
    m_ps1Settings.fov = 60.0f;
    m_ps1Settings.nearPlane = 0.1f;
    m_ps1Settings.farPlane = 100.0f;
    m_ps1Settings.noPerspectiveCorrection = true;
    m_ps1Settings.dithering = true;
    m_ps1Settings.colorQuantization = true;
    m_ps1Settings.colorBits = 16;
    
    // Disable modern rendering features
    m_settings.samplesMSAA = 1; // No MSAA
    m_settings.highDynamicPrecission = FloatPrecission::F16; // Lower precision
    m_settings.softwareAA = SoftwareAA::NONE; // No anti-aliasing
    m_settings.shadowQuality = 512; // Lower shadow resolution
    
    // Create PS1-specific pipelines
    create_ps1_pipeline();
    create_ps1_descriptor_set();
    create_post_processing_pipelines();
}

void PS1StyleRenderer::set_ps1_settings(const PS1Settings& settings) {
    m_ps1Settings = settings;
    create_ps1_pipeline(); // Recreate pipeline with new settings
}

void PS1StyleRenderer::on_before_render(Core::Scene* const scene) {
    // Call parent implementation
    ForwardRenderer::on_before_render(scene);
    
    // Apply PS1-specific pre-render setup
    if (scene) {
        // Quantize vertices for PS1-style precision
        auto meshes = scene->get_meshes();
        quantize_vertices(meshes, 0.1f);
        
        // Enable flat shading if requested
        if (m_ps1Settings.flatShading) {
            enable_flat_shading(meshes);
        }
    }
}

void PS1StyleRenderer::on_after_render(RenderResult& renderResult, Core::Scene* const scene) {
    // Call parent implementation
    ForwardRenderer::on_after_render(renderResult, scene);
    
    // Apply PS1-specific post-processing
    apply_ps1_post_processing();
}

void PS1StyleRenderer::render_ps1_scene(Core::Scene* const scene, Core::Camera* const camera) {
    if (!scene || !camera) return;
    
    // Use PS1-specific pipeline
    if (m_ps1Pipeline) {
        // Bind PS1 pipeline
        // Render scene with PS1 settings
        // This would integrate with your existing Vulkan rendering code
    }
}

void PS1StyleRenderer::apply_ps1_post_processing() {
    if (m_ps1Settings.dithering) {
        // Apply dithering effect
        // This would use the dither pipeline
    }
    
    if (m_ps1Settings.colorQuantization) {
        // Apply color quantization
        // This would use the quantize pipeline
    }
}

void PS1StyleRenderer::quantize_vertices(std::vector<Core::Mesh*>& meshes, float gridSize) {
    for (auto* mesh : meshes) {
        if (!mesh) continue;
        
        auto geometry = mesh->get_geometry();
        if (!geometry) continue;
        
        // Get vertex data
        auto vertices = geometry->get_vertices();
        
        // Quantize vertices to grid
        for (auto& vertex : vertices) {
            vertex.x = std::floor(vertex.x / gridSize) * gridSize;
            vertex.y = std::floor(vertex.y / gridSize) * gridSize;
            vertex.z = std::floor(vertex.z / gridSize) * gridSize;
        }
        
        // Update geometry
        geometry->set_vertices(vertices);
    }
}

void PS1StyleRenderer::enable_flat_shading(std::vector<Core::Mesh*>& meshes) {
    for (auto* mesh : meshes) {
        if (!mesh) continue;
        
        auto geometry = mesh->get_geometry();
        if (!geometry) continue;
        
        // Calculate flat normals for each face
        auto vertices = geometry->get_vertices();
        auto indices = geometry->get_indices();
        auto normals = geometry->get_normals();
        
        // For each triangle, calculate flat normal
        for (size_t i = 0; i < indices.size(); i += 3) {
            if (i + 2 >= indices.size()) break;
            
            Vec3 v0 = vertices[indices[i]];
            Vec3 v1 = vertices[indices[i + 1]];
            Vec3 v2 = vertices[indices[i + 2]];
            
            // Calculate face normal
            Vec3 edge1 = v1 - v0;
            Vec3 edge2 = v2 - v0;
            Vec3 faceNormal = math::normalize(math::cross(edge1, edge2));
            
            // Set same normal for all three vertices
            normals[indices[i]] = faceNormal;
            normals[indices[i + 1]] = faceNormal;
            normals[indices[i + 2]] = faceNormal;
        }
        
        // Update geometry
        geometry->set_normals(normals);
    }
}

void PS1StyleRenderer::create_ps1_pipeline() {
    // Create PS1-specific pipeline
    // This would integrate with your existing Vulkan pipeline creation code
    // Using the PS1 vertex and fragment shaders we created
}

void PS1StyleRenderer::create_ps1_descriptor_set() {
    // Create descriptor set for PS1 uniforms
    // This would integrate with your existing Vulkan descriptor set creation code
}

void PS1StyleRenderer::create_post_processing_pipelines() {
    // Create dithering and quantization pipelines
    // This would integrate with your existing Vulkan pipeline creation code
}

float PS1StyleRenderer::calculate_vertex_lighting(const Vec3& normal, const Vec3& lightDirection) {
    float lightDot = std::max(math::dot(math::normalize(normal), math::normalize(lightDirection)), 0.0f);
    
    if (m_ps1Settings.quantizedLighting) {
        lightDot = std::floor(lightDot * m_ps1Settings.lightingLevels) / m_ps1Settings.lightingLevels;
    }
    
    return lightDot;
}

float PS1StyleRenderer::calculate_vertex_fog(float depth, float fogStart, float fogEnd) {
    float fogFactor = std::clamp((depth - fogStart) / (fogEnd - fogStart), 0.0f, 1.0f);
    return std::pow(fogFactor, m_ps1Settings.fogDensity);
}

} // namespace Systems
} // namespace VKFW
