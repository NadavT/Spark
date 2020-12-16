
#pragma once
#include "platform/vulkan/drawables/vulkan_drawable.h"
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/vertex/vertex3d.h"

#include "spark/renderer/drawables/cube.h"

namespace Spark
{
class VulkanCube : public virtual Cube, public virtual VulkanDrawable
{
  public:
    virtual ~VulkanCube();
    VulkanCube(const VulkanCube &other);
    VulkanCube(VulkanCube &&other) noexcept;
    VulkanCube &operator=(const VulkanCube &other);
    VulkanCube &operator=(VulkanCube &&other) noexcept;

    virtual void fillCommandBuffer(VkCommandBuffer commandBuffer);

  protected:
    VulkanCube(VulkanRenderer &renderer, glm::vec3 position, glm::vec3 scale = glm::vec3(1.0));

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
};
} // namespace Spark
