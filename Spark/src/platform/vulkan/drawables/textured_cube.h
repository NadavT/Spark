#pragma once
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/resource/texture.h"
#include "platform/vulkan/resource/texture_sampler.h"
#include "platform/vulkan/vertex/vertex3d.h"

#include "spark/renderer/drawables/cube.h"

namespace Spark
{
class VulkanTexturedCube : public Cube
{
  public:
    VulkanTexturedCube(VulkanRenderer &renderer, glm::vec3 position, const VulkanTexture &texture,
                       const VulkanTexture &specularTexture, glm::vec3 scale = glm::vec3(1.0));
    virtual ~VulkanTexturedCube();
    VulkanTexturedCube(const VulkanTexturedCube &other);
    VulkanTexturedCube(VulkanTexturedCube &&other) noexcept;
    VulkanTexturedCube &operator=(const VulkanTexturedCube &other);
    VulkanTexturedCube &operator=(VulkanTexturedCube &&other) noexcept;

    void fillCommandBuffer(VkCommandBuffer commandBuffer);

    virtual const VulkanTexture &getTexture() const;
    virtual const VulkanTexture &getSpecularTexture() const;

  private:
    void copyCube(const VulkanTexturedCube &other);
    void moveCube(VulkanTexturedCube &other) noexcept;

  private:
    VulkanContext &m_context;
    VulkanRenderer &m_renderer;
    VkBuffer m_vertexBuffer;
    VkDeviceMemory m_vertexBufferMemory;
    VkDeviceSize m_verticesOffset;
    VkDeviceSize m_indicesOffset;
    const VulkanTexture &m_texture;
    const VulkanTexture &m_specularTexture;
};
} // namespace Spark
