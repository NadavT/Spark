#include "texture_sampler.h"

#include "spark/core/log.h"

namespace Spark::Render
{
VulkanTextureSampler::VulkanTextureSampler(VulkanContext &context, const std::string &name)
    : Resource(name, ResourceType::internal)
    , m_context(context)
    , m_textureSampler(VK_NULL_HANDLE)
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    SPARK_CORE_ASSERT(vkCreateSampler(m_context.m_device, &samplerInfo, nullptr, &m_textureSampler) == VK_SUCCESS,
                      "failed to create texture sampler!");
}

VulkanTextureSampler::~VulkanTextureSampler()
{
    vkDestroySampler(m_context.m_device, m_textureSampler, nullptr);
}

VkSampler VulkanTextureSampler::getSampler() const
{
    return m_textureSampler;
}
} // namespace Spark::Render
