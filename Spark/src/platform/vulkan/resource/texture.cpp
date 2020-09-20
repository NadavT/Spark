#include "texture.h"

namespace Spark
{
VulkanTexture::VulkanTexture(const std::string &name, const VulkanTextureImage &image,
                             const VulkanTextureSampler &sampler)
    : Texture(name)
    , m_image(image)
    , m_sampler(sampler)
{
}

VulkanTexture::~VulkanTexture()
{
}

const VulkanTextureImage &VulkanTexture::getImage() const
{
    return m_image;
}

const VulkanTextureSampler &VulkanTexture::getSampler() const
{
    return m_sampler;
}
} // namespace Spark
