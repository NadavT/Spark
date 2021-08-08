
#pragma once
#include "platform/vulkan/drawables/vulkan_drawable.h"
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/vertex/vertex3d.h"

#include "spark/renderer/drawables/pipe.h"

namespace Spark::Render
{
class VulkanPipe : public virtual Pipe, public virtual VulkanDrawablePrimitive
{
  public:
    virtual ~VulkanPipe();
    VulkanPipe(const VulkanPipe &other);
    VulkanPipe(VulkanPipe &&other) noexcept;
    VulkanPipe &operator=(const VulkanPipe &other);
    VulkanPipe &operator=(VulkanPipe &&other) noexcept;

    virtual void fillCommandBuffer(VkCommandBuffer commandBuffer) const;

  protected:
    VulkanPipe(VulkanRenderer &renderer, std::vector<glm::vec3> positions, float radius, int sectors, int stacks,
               glm::vec3 scale = glm::vec3(1.0));

  private:
    void fillVeticesAndIndices();

    void copyPipe(const VulkanPipe &other);
    void movePipe(VulkanPipe &other) noexcept;

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
