#include "textured_drawable.h"
namespace Spark
{
VulkanTexturedDrawable::VulkanTexturedDrawable(const VulkanTexture &texture, const VulkanTexture &specularTexture)
    : m_texture(texture)
    , m_specularTexture(specularTexture)
{
}

VulkanTexturedDrawable::VulkanTexturedDrawable(const VulkanTexturedDrawable &other)
    : m_texture(other.m_texture)
    , m_specularTexture(other.m_specularTexture)
{
}

VulkanTexturedDrawable::VulkanTexturedDrawable(VulkanTexturedDrawable &&other) noexcept
    : m_texture(other.m_texture)
    , m_specularTexture(other.m_specularTexture)
{
}

VulkanTexturedDrawable &VulkanTexturedDrawable::operator=(const VulkanTexturedDrawable &other)
{
    return *this;
}

VulkanTexturedDrawable &VulkanTexturedDrawable::operator=(VulkanTexturedDrawable &&other) noexcept
{
    return *this;
}

const VulkanTexture &VulkanTexturedDrawable::getTexture() const
{
    return m_texture;
}

const VulkanTexture &VulkanTexturedDrawable::getSpecularTexture() const
{
    return m_specularTexture;
}

VulkanDrawableType VulkanTexturedDrawable::getDrawableType() const
{
    return VulkanDrawableType::Textured;
}
} // namespace Spark