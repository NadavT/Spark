#pragma once

#include "platform/vulkan/renderer.h"

#include "spark/renderer/drawable.h"

namespace Spark
{

enum class VulkanDrawableType
{
    Colored,
    Textured
};
class VulkanDrawable : public virtual Drawable
{
  public:
    virtual ~VulkanDrawable() = default;

    virtual void fillCommandBuffer(VkCommandBuffer commandBuffer) = 0;
    virtual VulkanDrawableType getDrawableType() const = 0;

  protected:
    VulkanDrawable() = default;
};
} // namespace Spark