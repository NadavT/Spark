#include "mesh.h"

#include "spark/core/application.h"
#include "spark/core/log.h"

#ifdef SPARK_PLATFORM_VULKAN
    #include "platform/vulkan/renderer.h"
    #include "platform/vulkan/resource/mesh.h"
#endif // SPARK_PLATFORM_VULKAN

namespace Spark
{
Mesh::Mesh(std::vector<Vertex3D> vertices, std::vector<unsigned int> indices, struct MeshBaseColor baseColor,
           float shininess, std::vector<const Texture *> &textures, std::vector<const Texture *> &specularTextures)
    : m_vertices(vertices)
    , m_indices(indices)
    , m_baseColor(baseColor)
    , m_textures(textures)
    , m_specularTextures(specularTextures)
{
}

std::unique_ptr<Mesh> createMesh(std::vector<Vertex3D> vertices, std::vector<unsigned int> indices,
                                 struct MeshBaseColor baseColor, float shininess,
                                 std::vector<const Texture *> &textures, std::vector<const Texture *> &specularTextures)
{
#ifdef SPARK_PLATFORM_VULKAN
    Render::VulkanRenderer &renderer = reinterpret_cast<Render::VulkanRenderer &>(Application::GetApp().GetRenderer());
    return std::make_unique<Render::VulkanMesh>(renderer, vertices, indices, baseColor, shininess, textures,
                                                specularTextures);
#endif // SPARK_PLATFORM_VULKAN
}

const std::vector<const Texture *> &Mesh::getTextures() const
{
    return m_textures;
}

const std::vector<const Texture *> &Mesh::getSpecularTextures() const
{
    return m_specularTextures;
}

struct MeshBaseColor Mesh::getBaseColor() const
{
    return m_baseColor;
}

float Mesh::getShininess() const
{
    return m_shininess;
}

std::string Mesh::getTexturesID() const
{
    std::string id;
    for (const Texture *texture : m_textures)
    {
        id += texture->getName();
    }
    return id;
}

std::string Mesh::getSpecularTexturesID() const
{
    std::string id;
    for (const Texture *texture : m_specularTextures)
    {
        id += texture->getName();
    }
    return id;
}

const std::vector<Vertex3D> &Mesh::getVertices() const
{
    return m_vertices;
}
} // namespace Spark