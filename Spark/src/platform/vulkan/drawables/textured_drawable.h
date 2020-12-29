#pragma once
#include "platform/vulkan/drawables/vulkan_drawable.h"
#include "platform/vulkan/resource/texture.h"
#include "platform/vulkan/resource/texture_sampler.h"

namespace Spark::Render
{
class VulkanTexturedDrawable : public virtual VulkanDrawable
{
  public:
    virtual ~VulkanTexturedDrawable() = default;

    virtual VulkanDrawableType getDrawableType() const;

    virtual const VulkanTexture &getTexture() const;
    virtual const VulkanTexture &getSpecularTexture() const;

  protected:
    VulkanTexturedDrawable(const VulkanTexture &texture, const VulkanTexture &specularTexture);
    VulkanTexturedDrawable(const VulkanTexturedDrawable &other);
    VulkanTexturedDrawable(VulkanTexturedDrawable &&other) noexcept;
    VulkanTexturedDrawable &operator=(const VulkanTexturedDrawable &other);
    VulkanTexturedDrawable &operator=(VulkanTexturedDrawable &&other) noexcept;
    const VulkanTexture &m_texture;
    const VulkanTexture &m_specularTexture;
};
} // namespace Spark::Render