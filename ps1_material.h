#pragma once

#include <engine/core/materials/phong.h>
#include <memory>

namespace VKFW {
namespace Core {

struct PS1MaterialSettings {
    bool useTexture = true;
    bool useVertexColors = true;
    bool quantizedLighting = true;
    int lightingLevels = 4;
    bool useFog = true;
    float fogDensity = 1.0f;
    bool dithering = true;
    bool colorQuantization = true;
    int colorBits = 16;
    bool flatShading = false;
    bool affineTextureMapping = true;
};

class PS1Material : public IMaterial {
public:
    PS1Material();
    PS1Material(const std::string& name);
    ~PS1Material() = default;
    
    // PS1-specific setup
    void setup_ps1_style();
    void set_ps1_settings(const PS1MaterialSettings& settings);
    const PS1MaterialSettings& get_ps1_settings() const { return m_ps1Settings; }
    
    // Material properties
    void set_texture(std::shared_ptr<Graphics::Texture> texture);
    void set_color(const Vec3& color);
    void set_emissive(const Vec3& emissive);
    void set_metallic(float metallic);
    void set_roughness(float roughness);
    
    // PS1-specific rendering
    void bind_ps1_material();
    void update_ps1_uniforms();
    
    // Override base material methods
    void bind() override;
    void unbind() override;
    void update_uniforms() override;
    
    // Getters
    std::shared_ptr<Graphics::Texture> get_texture() const { return m_texture; }
    Vec3 get_color() const { return m_color; }
    Vec3 get_emissive() const { return m_emissive; }
    float get_metallic() const { return m_metallic; }
    float get_roughness() const { return m_roughness; }
    
private:
    PS1MaterialSettings m_ps1Settings;
    
    // Material properties
    std::shared_ptr<Graphics::Texture> m_texture;
    Vec3 m_color = Vec3(1.0f, 1.0f, 1.0f);
    Vec3 m_emissive = Vec3(0.0f, 0.0f, 0.0f);
    float m_metallic = 0.0f;
    float m_roughness = 0.5f;
    
    // PS1-specific uniforms
    std::shared_ptr<Graphics::Buffer> m_ps1UniformBuffer;
    std::shared_ptr<Graphics::DescriptorSet> m_ps1DescriptorSet;
    
    void create_ps1_uniform_buffer();
    void create_ps1_descriptor_set();
    void update_ps1_uniform_data();
};

} // namespace Core
} // namespace VKFW
