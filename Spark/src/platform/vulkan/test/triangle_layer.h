#ifndef SPARK_VULKAN_TRIANGLE_LAYER_H
#define SPARK_VULKAN_TRIANGLE_LAYER_H

#include "platform/vulkan/pipeline/pipeline.h"
#include "platform/vulkan/renderer.h"
#include "spark/layer/layer.h"

namespace Spark::Render
{
class VulkanTriangleLayer : public Layer
{
  public:
    VulkanTriangleLayer(VulkanRenderer &renderer);
    virtual ~VulkanTriangleLayer();

    virtual void OnAttach();
    virtual void OnDetach();
    virtual void OnRender();

  private:
    void createCommandBuffers();

  private:
    VulkanRenderer &m_renderer;
    VulkanFramebuffer *m_framebuffer;
    VulkanPipeline *m_pipeline;

    std::vector<VkCommandBuffer> m_commandBuffers;
};
} // namespace Spark::Render

#endif /* SPARK_VULKAN_TRIANGLE_LAYER_H */
