#pragma once

#include "spark/resource/texture.h"

#include "platform/vulkan/resource/texture_image.h"
#include "platform/vulkan/resource/texture_sampler.h"
#include "platform/vulkan/vulkan_context.h"

namespace Spark::Render
{
class VulkanTexture : public Texture
{
  public:
    VulkanTexture(const std::string &name, const VulkanTextureImage &image, const VulkanTextureSampler &sampler);
    virtual ~VulkanTexture();

    const VulkanTextureImage &getImage() const;
    const VulkanTextureSampler &getSampler() const;

  private:
    const VulkanTextureImage &m_image;
    const VulkanTextureSampler &m_sampler;
};
} // namespace Spark::Render
