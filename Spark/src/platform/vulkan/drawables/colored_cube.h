#pragma once
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/vertex/vertex3d.h"

#include "spark/renderer/drawables/cube.h"

namespace Spark
{
class VulkanColoredCube : public Cube
{
  public:
    VulkanColoredCube(VulkanRenderer &renderer, glm::vec3 position, glm::vec3 color, glm::vec3 scale = glm::vec3(1.0));
    virtual ~VulkanColoredCube();
    VulkanColoredCube(const VulkanColoredCube &other);
    VulkanColoredCube(VulkanColoredCube &&other) noexcept;
    VulkanColoredCube &operator=(const VulkanColoredCube &other);
    VulkanColoredCube &operator=(VulkanColoredCube &&other) noexcept;

    void fillCommandBuffer(VkCommandBuffer commandBuffer);

    virtual const glm::vec3 &getColor() const;

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
    glm::vec3 m_color;
};
} // namespace Spark
