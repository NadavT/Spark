
#pragma once
#include "platform/vulkan/drawables/vulkan_drawable.h"
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/vertex/vertex3d.h"

#include "spark/renderer/drawables/sphere.h"

namespace Spark::Render
{
class VulkanSphere : public virtual Sphere, public virtual VulkanDrawable
{
  public:
    virtual ~VulkanSphere();
    VulkanSphere(const VulkanSphere &other);
    VulkanSphere(VulkanSphere &&other) noexcept;
    VulkanSphere &operator=(const VulkanSphere &other);
    VulkanSphere &operator=(VulkanSphere &&other) noexcept;

    virtual void fillCommandBuffer(VkCommandBuffer commandBuffer);

  protected:
    VulkanSphere(VulkanRenderer &renderer, glm::vec3 position, int sectors, int stacks,
                 glm::vec3 scale = glm::vec3(1.0));

  private:
    void fillVeticesAndIndices();

    void copySphere(const VulkanSphere &other);
    void moveSphere(VulkanSphere &other) noexcept;

  private:
    VulkanContext &m_context;
    VulkanRenderer &m_renderer;
    VkBuffer m_vertexBuffer;
    VkDeviceMemory m_vertexBufferMemory;
    VkDeviceSize m_verticesOffset;
    VkDeviceSize m_indicesOffset;
    std::vector<Vertex3D> m_vertices;
    std::vector<uint32_t> m_indices;
};
} // namespace Spark::Render
