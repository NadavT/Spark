#ifndef SPARK_VULKAN_FRAMEBUFFER3D_H
#define SPARK_VULKAN_FRAMEBUFFER3D_H

#include "platform/vulkan/framebuffer/framebuffer.h"

namespace Spark::Render
{
class VulkanFramebuffer3D : public VulkanFramebuffer
{
  public:
    VulkanFramebuffer3D(VulkanContext &context, VkImageView renderTarget = VK_NULL_HANDLE, bool clear = false,
                        bool resolve = false);
    virtual ~VulkanFramebuffer3D();

    virtual void cleanup();
    virtual void recreate(VkImageView renderTarget);

    virtual VkImage getDepthImage() const;
    virtual VkImageView getDepthImageView() const;
    virtual VkRect2D getDepthImageDimentions() const;

  private:
    void createDepthImage();
    void createRenderPass();
    void createSwapchainFramebuffers();

  private:
    VkImage m_depthImage;
    VkDeviceMemory m_depthImageMemory;
    VkImageView m_depthImageView;
};
} // namespace Spark::Render

#endif /* SPARK_VULKAN_FRAMEBUFFER3D_H */
