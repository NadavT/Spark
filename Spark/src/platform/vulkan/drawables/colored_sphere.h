#pragma once
#include "platform/vulkan/drawables/colored_drawable.h"
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/vertex/vertex3d.h"

#include "spark/renderer/drawables/sphere.h"

namespace Spark
{
class VulkanColoredSphere : public virtual Sphere, public virtual VulkanColoredDrawable
{
  public:
    VulkanColoredSphere(VulkanRenderer &renderer, glm::vec3 position, glm::vec3 color,
                        glm::vec3 scale = glm::vec3(1.0));
    virtual ~VulkanColoredSphere();
    VulkanColoredSphere(const VulkanColoredSphere &other);
    VulkanColoredSphere(VulkanColoredSphere &&other) noexcept;
    VulkanColoredSphere &operator=(const VulkanColoredSphere &other);
    VulkanColoredSphere &operator=(VulkanColoredSphere &&other) noexcept;

    virtual void fillCommandBuffer(VkCommandBuffer commandBuffer);

  private:
    void copySphere(const VulkanColoredSphere &other);
    void moveSphere(VulkanColoredSphere &other) noexcept;

  private:
    VulkanContext &m_context;
    VulkanRenderer &m_renderer;
    VkBuffer m_vertexBuffer;
    VkDeviceMemory m_vertexBufferMemory;
    VkDeviceSize m_verticesOffset;
    VkDeviceSize m_indicesOffset;
};
} // namespace Spark
