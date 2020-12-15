#pragma once
#include "platform/vulkan/drawables/vulkan_drawable.h"

#include <glm/glm.hpp>

namespace Spark
{
class VulkanColoredDrawable : public virtual VulkanDrawable
{
  public:
    virtual ~VulkanColoredDrawable() = default;

    virtual VulkanDrawableType getDrawableType() const;

    virtual const glm::vec3 &getColor() const;
    virtual void setColor(glm::vec3 color);

  protected:
    VulkanColoredDrawable(glm::vec3 color);
    VulkanColoredDrawable(const VulkanColoredDrawable &other);
    VulkanColoredDrawable(VulkanColoredDrawable &&other) noexcept;
    VulkanColoredDrawable &operator=(const VulkanColoredDrawable &other);
    VulkanColoredDrawable &operator=(VulkanColoredDrawable &&other) noexcept;
    glm::vec3 m_color;
};
} // namespace Spark