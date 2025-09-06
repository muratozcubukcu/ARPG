#pragma once

#include <engine/core/scene/mesh.h>
#include <engine/core/geometries/geometry.h>
#include <memory>

namespace VKFW {
namespace Core {

struct PS1MeshSettings {
    bool affineTextureMapping = true;
    bool vertexColors = true;
    bool flatShading = false;
    bool quantizedVertices = true;
    float vertexGridSize = 0.1f;
    bool lowPolyStyle = true;
    int maxVerticesPerFace = 3; // Force triangular faces
};

class PS1Mesh : public Mesh {
public:
    PS1Mesh();
    PS1Mesh(const std::string& name);
    ~PS1Mesh() = default;
    
    // PS1-specific setup
    void setup_ps1_style();
    void set_ps1_settings(const PS1MeshSettings& settings);
    const PS1MeshSettings& get_ps1_settings() const { return m_ps1Settings; }
    
    // PS1-specific geometry operations
    void quantize_vertices(float gridSize = 0.1f);
    void enable_flat_shading();
    void enable_vertex_colors();
    void convert_to_low_poly();
    void apply_ps1_style_geometry();
    
    // Override base mesh methods
    void set_geometry(IGeometry* geometry) override;
    PS1Mesh* clone() const override;
    
    // PS1-specific rendering
    void render_ps1_style();
    void update_ps1_attributes();
    
private:
    PS1MeshSettings m_ps1Settings;
    std::vector<Vec3> m_originalVertices;
    std::vector<Vec3> m_originalNormals;
    
    void backup_original_geometry();
    void restore_original_geometry();
    void calculate_flat_normals();
    void quantize_vertex_data(std::vector<Vec3>& vertices, float gridSize);
    void ensure_triangular_faces();
};

} // namespace Core
} // namespace VKFW
