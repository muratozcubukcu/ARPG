#include "ps1_mesh.h"
#include "graphics/Vulkan-Engine/src/core/geometries/geometry.h"
#include <algorithm>
#include <cmath>

namespace VKFW {
namespace Core {

PS1Mesh::PS1Mesh() : Mesh() {
    setup_ps1_style();
}

PS1Mesh::PS1Mesh(const std::string& name) : Mesh(name) {
    setup_ps1_style();
}

void PS1Mesh::setup_ps1_style() {
    // Configure PS1-style settings
    m_ps1Settings.affineTextureMapping = true;
    m_ps1Settings.vertexColors = true;
    m_ps1Settings.flatShading = false;
    m_ps1Settings.quantizedVertices = true;
    m_ps1Settings.vertexGridSize = 0.1f;
    m_ps1Settings.lowPolyStyle = true;
    m_ps1Settings.maxVerticesPerFace = 3;
}

void PS1Mesh::set_ps1_settings(const PS1MeshSettings& settings) {
    m_ps1Settings = settings;
    apply_ps1_style_geometry();
}

void PS1Mesh::quantize_vertices(float gridSize) {
    if (!m_geometry) return;
    
    auto vertices = m_geometry->get_vertices();
    quantize_vertex_data(vertices, gridSize);
    m_geometry->set_vertices(vertices);
}

void PS1Mesh::enable_flat_shading() {
    if (!m_geometry) return;
    
    backup_original_geometry();
    calculate_flat_normals();
}

void PS1Mesh::enable_vertex_colors() {
    if (!m_geometry) return;
    
    // Ensure vertex colors are available
    auto colors = m_geometry->get_colors();
    if (colors.empty()) {
        // Generate default vertex colors
        auto vertices = m_geometry->get_vertices();
        colors.resize(vertices.size(), Vec3(1.0f, 1.0f, 1.0f));
        m_geometry->set_colors(colors);
    }
}

void PS1Mesh::convert_to_low_poly() {
    if (!m_geometry) return;
    
    backup_original_geometry();
    ensure_triangular_faces();
    
    // Simplify geometry for low-poly look
    auto vertices = m_geometry->get_vertices();
    auto normals = m_geometry->get_normals();
    auto indices = m_geometry->get_indices();
    
    // Remove duplicate vertices
    std::vector<Vec3> uniqueVertices;
    std::vector<Vec3> uniqueNormals;
    std::vector<uint32_t> newIndices;
    
    for (size_t i = 0; i < indices.size(); ++i) {
        Vec3 vertex = vertices[indices[i]];
        Vec3 normal = normals[indices[i]];
        
        // Find if vertex already exists
        bool found = false;
        for (size_t j = 0; j < uniqueVertices.size(); ++j) {
            if (math::length(vertex - uniqueVertices[j]) < 0.001f) {
                newIndices.push_back(static_cast<uint32_t>(j));
                found = true;
                break;
            }
        }
        
        if (!found) {
            uniqueVertices.push_back(vertex);
            uniqueNormals.push_back(normal);
            newIndices.push_back(static_cast<uint32_t>(uniqueVertices.size() - 1));
        }
    }
    
    m_geometry->set_vertices(uniqueVertices);
    m_geometry->set_normals(uniqueNormals);
    m_geometry->set_indices(newIndices);
}

void PS1Mesh::apply_ps1_style_geometry() {
    if (!m_geometry) return;
    
    backup_original_geometry();
    
    if (m_ps1Settings.quantizedVertices) {
        quantize_vertices(m_ps1Settings.vertexGridSize);
    }
    
    if (m_ps1Settings.flatShading) {
        enable_flat_shading();
    }
    
    if (m_ps1Settings.vertexColors) {
        enable_vertex_colors();
    }
    
    if (m_ps1Settings.lowPolyStyle) {
        convert_to_low_poly();
    }
}

void PS1Mesh::set_geometry(IGeometry* geometry) {
    Mesh::set_geometry(geometry);
    apply_ps1_style_geometry();
}

PS1Mesh* PS1Mesh::clone() const {
    PS1Mesh* mesh = new PS1Mesh();
    mesh->m_material = m_material;
    mesh->m_geometry = m_geometry;
    mesh->m_ps1Settings = m_ps1Settings;
    mesh->m_originalVertices = m_originalVertices;
    mesh->m_originalNormals = m_originalNormals;
    mesh->setup_volume();
    mesh->set_name(m_name + " clone");
    mesh->set_transform(m_transform);
    m_instanceCount++;
    return mesh;
}

void PS1Mesh::render_ps1_style() {
    // PS1-specific rendering logic
    // This would integrate with your existing Vulkan rendering code
}

void PS1Mesh::update_ps1_attributes() {
    // Update PS1-specific attributes
    // This would be called when mesh properties change
}

void PS1Mesh::backup_original_geometry() {
    if (!m_geometry) return;
    
    m_originalVertices = m_geometry->get_vertices();
    m_originalNormals = m_geometry->get_normals();
}

void PS1Mesh::restore_original_geometry() {
    if (!m_geometry) return;
    
    m_geometry->set_vertices(m_originalVertices);
    m_geometry->set_normals(m_originalNormals);
}

void PS1Mesh::calculate_flat_normals() {
    if (!m_geometry) return;
    
    auto vertices = m_geometry->get_vertices();
    auto indices = m_geometry->get_indices();
    auto normals = m_geometry->get_normals();
    
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
    
    m_geometry->set_normals(normals);
}

void PS1Mesh::quantize_vertex_data(std::vector<Vec3>& vertices, float gridSize) {
    for (auto& vertex : vertices) {
        vertex.x = std::floor(vertex.x / gridSize) * gridSize;
        vertex.y = std::floor(vertex.y / gridSize) * gridSize;
        vertex.z = std::floor(vertex.z / gridSize) * gridSize;
    }
}

void PS1Mesh::ensure_triangular_faces() {
    if (!m_geometry) return;
    
    auto indices = m_geometry->get_indices();
    std::vector<uint32_t> newIndices;
    
    // Convert all faces to triangles
    for (size_t i = 0; i < indices.size(); i += 3) {
        if (i + 2 >= indices.size()) break;
        
        // Already triangular
        newIndices.push_back(indices[i]);
        newIndices.push_back(indices[i + 1]);
        newIndices.push_back(indices[i + 2]);
    }
    
    m_geometry->set_indices(newIndices);
}

} // namespace Core
} // namespace VKFW
