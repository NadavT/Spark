#pragma once
#include "platform/vulkan/drawables/render_primitive.h"
#include "platform/vulkan/drawables/vulkan_drawable.h"
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/resource/texture.h"
#include "platform/vulkan/resource/texture_sampler.h"
#include "platform/vulkan/vertex/vertex2d.h"

#include "spark/renderer/drawables/quad.h"

namespace Spark::Render
{
class VulkanQuad : public virtual Quad, public virtual VulkanDrawablePrimitive
{
  public:
    VulkanQuad(VulkanRenderer &renderer, glm::vec2 position, const VulkanTexture &texture,
               glm::vec2 scale = glm::vec2(1.0));
    virtual ~VulkanQuad();
    VulkanQuad(const VulkanQuad &other);
    VulkanQuad(VulkanQuad &&other) noexcept;
    VulkanQuad &operator=(const VulkanQuad &other);
    VulkanQuad &operator=(VulkanQuad &&other) noexcept;

    virtual void fillCommandBuffer(VkCommandBuffer commandBuffer) const;
    virtual VulkanDrawableType getDrawableType() const;

    virtual const VulkanTexture &getTexture() const;

  private:
    void copyQuad(const VulkanQuad &other);
    void moveQuad(VulkanQuad &other) noexcept;

  private:
    VulkanContext &m_context;
    VulkanRenderer &m_renderer;
    VkBuffer m_vertexBuffer;
    VkDeviceMemory m_vertexBufferMemory;
    VkDeviceSize m_verticesOffset;
    VkDeviceSize m_indicesOffset;
    const VulkanTexture &m_texture;
};
} // namespace Spark::Render
