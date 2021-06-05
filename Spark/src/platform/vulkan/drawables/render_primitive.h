#pragma once

#include "platform/vulkan/renderer.h"

namespace Spark::Render
{
class VulkanRenderPrimitive
{
  public:
    virtual ~VulkanRenderPrimitive() = default;

    virtual void fillCommandBuffer(VkCommandBuffer commandBuffer) const = 0;

  protected:
    VulkanRenderPrimitive() = default;
};
} // namespace Spark::Render
