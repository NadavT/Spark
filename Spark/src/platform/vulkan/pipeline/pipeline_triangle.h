#ifndef SPARK_VULKAN_PIPELINE_TRIANGLE_H
#define SPARK_VULKAN_PIPELINE_TRIANGLE_H

#include "platform/vulkan/pipeline/pipeline.h"

namespace Spark::Render
{
class VulkanPipelineTriangle : public VulkanPipeline
{
  public:
    VulkanPipelineTriangle(VulkanContext &context, VulkanFramebuffer &framebuffer);
    virtual ~VulkanPipelineTriangle();

    virtual void cleanup();
    virtual void recreate();

  private:
    VkPipelineLayout m_pipelineLayout;
};
} // namespace Spark::Render

#endif /* SPARK_VULKAN_PIPELINE_TRIANGLE_H */
