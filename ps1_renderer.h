#pragma once

#include <engine/systems/renderers/forward.h>
#include <engine/core/scene/camera.h>
#include <engine/core/scene/mesh.h>
#include <memory>
#include <vector>

namespace VKFW {
namespace Systems {

struct PS1Settings {
    // Rendering settings
    bool affineTextureMapping = true;
    bool vertexLighting = true;
    bool quantizedLighting = true;
    int lightingLevels = 4;
    bool vertexFog = true;
    float fogDensity = 1.0f;
    bool useVertexColors = true;
    bool flatShading = false;
    
    // Visual settings
    float fov = 60.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    bool noPerspectiveCorrection = true;
    
    // Post-processing
    bool dithering = true;
    bool colorQuantization = true;
    int colorBits = 16; // 16-bit color depth
};

class PS1StyleRenderer : public ForwardRenderer {
public:
    PS1StyleRenderer();
    ~PS1StyleRenderer() = default;
    
    // PS1-specific initialization
    void setup_ps1_style();
    void set_ps1_settings(const PS1Settings& settings);
    const PS1Settings& get_ps1_settings() const { return m_ps1Settings; }
    
    // Rendering overrides
    void on_before_render(Core::Scene* const scene) override;
    void on_after_render(RenderResult& renderResult, Core::Scene* const scene) override;
    
    // PS1-specific rendering
    void render_ps1_scene(Core::Scene* const scene, Core::Camera* const camera);
    void apply_ps1_post_processing();
    
    // Low-poly model support
    void quantize_vertices(std::vector<Core::Mesh*>& meshes, float gridSize = 0.1f);
    void enable_flat_shading(std::vector<Core::Mesh*>& meshes);
    
private:
    PS1Settings m_ps1Settings;
    
    // PS1-specific resources
    std::shared_ptr<Graphics::Pipeline> m_ps1Pipeline;
    std::shared_ptr<Graphics::DescriptorSet> m_ps1DescriptorSet;
    
    // Post-processing resources
    std::shared_ptr<Graphics::Pipeline> m_ditherPipeline;
    std::shared_ptr<Graphics::Pipeline> m_quantizePipeline;
    
    void create_ps1_pipeline();
    void create_ps1_descriptor_set();
    void create_post_processing_pipelines();
    
    // PS1-style lighting calculation
    float calculate_vertex_lighting(const Vec3& normal, const Vec3& lightDirection);
    
    // PS1-style fog calculation
    float calculate_vertex_fog(float depth, float fogStart, float fogEnd);
};

} // namespace Systems
} // namespace VKFW
