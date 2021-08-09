#ifndef SPARK_VULKAN_CYLINDER_H
#define SPARK_VULKAN_CYLINDER_H

#include "platform/vulkan/drawables/vulkan_drawable.h"
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/vertex/vertex3d.h"

#include "spark/renderer/drawables/cylinder.h"

namespace Spark::Render
{
class VulkanCylinder : public virtual Cylinder, public virtual VulkanDrawablePrimitive
{
  public:
    virtual ~VulkanCylinder();
    VulkanCylinder(const VulkanCylinder &other);
    VulkanCylinder(VulkanCylinder &&other) noexcept;
    VulkanCylinder &operator=(const VulkanCylinder &other);
    VulkanCylinder &operator=(VulkanCylinder &&other) noexcept;

    virtual void fillCommandBuffer(VkCommandBuffer commandBuffer) const;

  protected:
    VulkanCylinder(VulkanRenderer &renderer, glm::vec3 position, float baseRadius, float topRadius, float height,
                   int sectors, int stacks, glm::vec3 scale = glm::vec3(1.0));

  private:
    void fillVeticesAndIndices();

    void copyCylinder(const VulkanCylinder &other);
    void moveCylinder(VulkanCylinder &other) noexcept;

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

#endif /* SPARK_VULKAN_CYLINDER_H */
