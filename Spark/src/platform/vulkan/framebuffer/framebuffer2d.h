#pragma once

#include "platform/vulkan/framebuffer/framebuffer.h"

namespace Spark::Render
{
class VulkanFramebuffer2D : public VulkanFramebuffer
{
  public:
    VulkanFramebuffer2D(VulkanContext &context, VkImageView renderTarget = VK_NULL_HANDLE, bool clear = false,
                        bool resolve = false);
    virtual ~VulkanFramebuffer2D();

    virtual void cleanup();
    virtual void recreate(VkImageView renderTarget);

  private:
    void createRenderPass();
    void createSwapchainFramebuffers();
};
} // namespace Spark::Render
