#ifndef SPARK_VULKAN_TEXTURE_SAMPLER_H
#define SPARK_VULKAN_TEXTURE_SAMPLER_H

#include "spark/resource/resource.h"

#include "platform/vulkan/vulkan_context.h"

namespace Spark::Render
{
class VulkanTextureSampler : public Resource
{
  public:
    VulkanTextureSampler(VulkanContext &context, const std::string &name);
    virtual ~VulkanTextureSampler();

    VkSampler getSampler() const;

  private:
    VulkanContext &m_context;
    VkSampler m_textureSampler;
};
} // namespace Spark::Render

#endif /* SPARK_VULKAN_TEXTURE_SAMPLER_H */
