#include "ps1_material.h"
#include "graphics/Vulkan-Engine/src/graphics/buffer.h"
#include "graphics/Vulkan-Engine/src/graphics/descriptors.h"
#include "graphics/Vulkan-Engine/src/graphics/texture.h"

namespace VKFW {
namespace Core {

PS1Material::PS1Material() : IMaterial() {
    setup_ps1_style();
}

PS1Material::PS1Material(const std::string& name) : IMaterial(name) {
    setup_ps1_style();
}

void PS1Material::setup_ps1_style() {
    // Configure PS1-style material settings
    m_ps1Settings.useTexture = true;
    m_ps1Settings.useVertexColors = true;
    m_ps1Settings.quantizedLighting = true;
    m_ps1Settings.lightingLevels = 4;
    m_ps1Settings.useFog = true;
    m_ps1Settings.fogDensity = 1.0f;
    m_ps1Settings.dithering = true;
    m_ps1Settings.colorQuantization = true;
    m_ps1Settings.colorBits = 16;
    m_ps1Settings.flatShading = false;
    m_ps1Settings.affineTextureMapping = true;
    
    // Initialize material properties
    m_color = Vec3(1.0f, 1.0f, 1.0f);
    m_emissive = Vec3(0.0f, 0.0f, 0.0f);
    m_metallic = 0.0f;
    m_roughness = 0.5f;
    
    // Create PS1-specific resources
    create_ps1_uniform_buffer();
    create_ps1_descriptor_set();
}

void PS1Material::set_ps1_settings(const PS1MaterialSettings& settings) {
    m_ps1Settings = settings;
    update_ps1_uniforms();
}

void PS1Material::set_texture(std::shared_ptr<Graphics::Texture> texture) {
    m_texture = texture;
    update_ps1_uniforms();
}

void PS1Material::set_color(const Vec3& color) {
    m_color = color;
    update_ps1_uniforms();
}

void PS1Material::set_emissive(const Vec3& emissive) {
    m_emissive = emissive;
    update_ps1_uniforms();
}

void PS1Material::set_metallic(float metallic) {
    m_metallic = std::clamp(metallic, 0.0f, 1.0f);
    update_ps1_uniforms();
}

void PS1Material::set_roughness(float roughness) {
    m_roughness = std::clamp(roughness, 0.0f, 1.0f);
    update_ps1_uniforms();
}

void PS1Material::bind_ps1_material() {
    // Bind PS1-specific material resources
    if (m_ps1DescriptorSet) {
        // Bind descriptor set
    }
    
    if (m_texture) {
        // Bind texture
    }
}

void PS1Material::update_ps1_uniforms() {
    update_ps1_uniform_data();
}

void PS1Material::bind() {
    // Override base bind to use PS1-specific binding
    bind_ps1_material();
}

void PS1Material::unbind() {
    // PS1-specific unbinding
}

void PS1Material::update_uniforms() {
    // Override base update to use PS1-specific uniforms
    update_ps1_uniforms();
}

void PS1Material::create_ps1_uniform_buffer() {
    // Create uniform buffer for PS1-specific uniforms
    // This would integrate with your existing Vulkan buffer creation code
}

void PS1Material::create_ps1_descriptor_set() {
    // Create descriptor set for PS1-specific uniforms
    // This would integrate with your existing Vulkan descriptor set creation code
}

void PS1Material::update_ps1_uniform_data() {
    if (!m_ps1UniformBuffer) return;
    
    // Update uniform buffer with PS1-specific data
    struct PS1UniformData {
        Vec3 color;
        Vec3 emissive;
        float metallic;
        float roughness;
        int useTexture;
        int useVertexColors;
        int quantizedLighting;
        int lightingLevels;
        int useFog;
        float fogDensity;
        int dithering;
        int colorQuantization;
        int colorBits;
        int flatShading;
        int affineTextureMapping;
    };
    
    PS1UniformData uniformData;
    uniformData.color = m_color;
    uniformData.emissive = m_emissive;
    uniformData.metallic = m_metallic;
    uniformData.roughness = m_roughness;
    uniformData.useTexture = m_ps1Settings.useTexture ? 1 : 0;
    uniformData.useVertexColors = m_ps1Settings.useVertexColors ? 1 : 0;
    uniformData.quantizedLighting = m_ps1Settings.quantizedLighting ? 1 : 0;
    uniformData.lightingLevels = m_ps1Settings.lightingLevels;
    uniformData.useFog = m_ps1Settings.useFog ? 1 : 0;
    uniformData.fogDensity = m_ps1Settings.fogDensity;
    uniformData.dithering = m_ps1Settings.dithering ? 1 : 0;
    uniformData.colorQuantization = m_ps1Settings.colorQuantization ? 1 : 0;
    uniformData.colorBits = m_ps1Settings.colorBits;
    uniformData.flatShading = m_ps1Settings.flatShading ? 1 : 0;
    uniformData.affineTextureMapping = m_ps1Settings.affineTextureMapping ? 1 : 0;
    
    // Update buffer data
    // This would integrate with your existing Vulkan buffer update code
}

} // namespace Core
} // namespace VKFW
