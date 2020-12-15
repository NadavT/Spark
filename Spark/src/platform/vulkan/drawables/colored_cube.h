#pragma once
#include "platform/vulkan/drawables/colored_drawable.h"
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/vertex/vertex3d.h"

#include "spark/renderer/drawables/cube.h"

namespace Spark
{
class VulkanColoredCube : public virtual Cube, public virtual VulkanColoredDrawable
{
  public:
    VulkanColoredCube(VulkanRenderer &renderer, glm::vec3 position, glm::vec3 color, glm::vec3 scale = glm::vec3(1.0));
    virtual ~VulkanColoredCube();
    VulkanColoredCube(const VulkanColoredCube &other);
    VulkanColoredCube(VulkanColoredCube &&other) noexcept;
    VulkanColoredCube &operator=(const VulkanColoredCube &other);
    VulkanColoredCube &operator=(VulkanColoredCube &&other) noexcept;

    virtual void fillCommandBuffer(VkCommandBuffer commandBuffer);

    virtual void highlight();
    virtual void unhighlight();

  private:
    void copyCube(const VulkanColoredCube &other);
    void moveCube(VulkanColoredCube &other) noexcept;

  private:
    VulkanContext &m_context;
    VulkanRenderer &m_renderer;
    VkBuffer m_vertexBuffer;
    VkDeviceMemory m_vertexBufferMemory;
    VkDeviceSize m_verticesOffset;
    VkDeviceSize m_indicesOffset;
};
} // namespace Spark
