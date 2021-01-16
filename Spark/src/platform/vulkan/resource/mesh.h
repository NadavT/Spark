#pragma once

#include "platform/vulkan/renderer.h"

#include "spark/resource/mesh.h"

namespace Spark::Render
{
class VulkanMesh : public Mesh
{
  public:
    VulkanMesh(VulkanRenderer &renderer, std::vector<Vertex3D> vertices, std::vector<unsigned int> indices,
               struct MeshBaseColor baseColor, float shininess, std::vector<const Texture *> &textures,
               std::vector<const Texture *> &specularTextures);
    virtual ~VulkanMesh();
    VulkanMesh(const VulkanMesh &other);
    VulkanMesh(VulkanMesh &&other) noexcept;
    VulkanMesh &operator=(const VulkanMesh &other);
    VulkanMesh &operator=(VulkanMesh &&other) noexcept;

    virtual void fillCommandBuffer(VkCommandBuffer commandBuffer);

  private:
    void copyMesh(const VulkanMesh &other);
    void moveMesh(VulkanMesh &other) noexcept;

  private:
    VulkanRenderer &m_renderer;
    VkBuffer m_vertexBuffer;
    VkDeviceMemory m_vertexBufferMemory;
    VkDeviceSize m_verticesOffset;
    VkDeviceSize m_indicesOffset;
};
} // namespace Spark::Render
