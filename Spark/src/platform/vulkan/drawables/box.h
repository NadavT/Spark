
#pragma once
#include "platform/vulkan/drawables/vulkan_drawable.h"
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/vertex/vertex3d.h"

#include "spark/renderer/drawables/box.h"

namespace Spark::Render
{
class VulkanBox : public virtual Box, public virtual VulkanDrawable
{
  public:
    virtual ~VulkanBox();
    VulkanBox(const VulkanBox &other);
    VulkanBox(VulkanBox &&other) noexcept;
    VulkanBox &operator=(const VulkanBox &other);
    VulkanBox &operator=(VulkanBox &&other) noexcept;

    virtual void fillCommandBuffer(VkCommandBuffer commandBuffer);

  protected:
    VulkanBox(VulkanRenderer &renderer, glm::vec3 position, glm::vec3 scale = glm::vec3(1.0));

  private:
    void copyCube(const VulkanBox &other);
    void moveCube(VulkanBox &other) noexcept;

  private:
    VulkanContext &m_context;
    VulkanRenderer &m_renderer;
    VkBuffer m_vertexBuffer;
    VkDeviceMemory m_vertexBufferMemory;
    VkDeviceSize m_verticesOffset;
    VkDeviceSize m_indicesOffset;
};
} // namespace Spark::Render
