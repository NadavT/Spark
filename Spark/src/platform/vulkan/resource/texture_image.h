#ifndef SPARK_VULKAN_TEXTURE_IMAGE_H
#define SPARK_VULKAN_TEXTURE_IMAGE_H

#include "spark/resource/resource.h"

#include "platform/vulkan/resource/texture_sampler.h"
#include "platform/vulkan/vulkan_context.h"

namespace Spark::Render
{
class VulkanTextureImage : public Resource
{
  public:
    VulkanTextureImage(VulkanContext &context, const std::string &name, const std::string &path);
    virtual ~VulkanTextureImage();

    VkImageView getImageView() const;

  private:
    VulkanContext &m_context;
    VkImage m_textureImage;
    VkDeviceMemory m_textureImageMemory;
    VkImageView m_textureImageView;
};
} // namespace Spark::Render

#endif /* SPARK_VULKAN_TEXTURE_IMAGE_H */
