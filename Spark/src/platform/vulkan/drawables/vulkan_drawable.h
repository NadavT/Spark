#pragma once

#include "platform/vulkan/renderer.h"

#include "spark/renderer/drawable.h"

namespace Spark
{
class VulkanDrawable : public virtual Drawable
{
  public:
    virtual ~VulkanDrawable() = default;

    virtual void fillCommandBuffer(VkCommandBuffer commandBuffer) = 0;

  protected:
    VulkanDrawable() = default;
};
} // namespace Spark