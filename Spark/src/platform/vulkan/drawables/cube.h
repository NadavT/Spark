#pragma once
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/resource/texture.h"
#include "platform/vulkan/resource/texture_sampler.h"
#include "platform/vulkan/vertex/vertex3d.h"

#include "spark/renderer/drawables/cube.h"

namespace Spark
{
class VulkanCube : public Cube
{
  public:
    VulkanCube(VulkanRenderer &renderer, glm::vec3 position, const VulkanTexture &texture,
               const VulkanTexture &specularTexture, glm::vec3 scale = glm::vec3(1.0));
    virtual ~VulkanCube();
    VulkanCube(const VulkanCube &other);
    VulkanCube(VulkanCube &&other) noexcept;
    VulkanCube &operator=(const VulkanCube &other);
    VulkanCube &operator=(VulkanCube &&other) noexcept;

    void fillCommandBuffer(VkCommandBuffer commandBuffer);

    virtual const VulkanTexture &getTexture() const;
    virtual const VulkanTexture &getSpecularTexture() const;

  private:
    void copyCube(const VulkanCube &other);
    void moveCube(VulkanCube &other) noexcept;

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
