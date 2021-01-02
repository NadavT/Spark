#include "textured_box.h"

#include "platform/vulkan/renderer.h"

#include "spark/core/application.h"

namespace Spark::Render
{
VulkanTexturedBox::VulkanTexturedBox(VulkanRenderer &renderer, glm::vec3 position, const VulkanTexture &texture,
                                     const VulkanTexture &specularTexture, glm::vec3 scale)
    : VulkanBox(renderer, position, scale)
    , VulkanTexturedDrawable(texture, specularTexture)
    , Box(position, scale)
    , Drawable3D(position, scale)
{
}
} // namespace Spark::Render
