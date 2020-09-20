#pragma once

#include "platform/vulkan/pipeline/pipeline.h"

namespace Spark
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
} // namespace Spark
