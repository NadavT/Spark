#include "textured_box.h"

namespace Spark::Render
{
VulkanTexturedBox::VulkanTexturedBox(VulkanRenderer &renderer, glm::vec3 position, const VulkanTexture &texture,
                                     const VulkanTexture &specularTexture, glm::vec3 scale)
    : Drawable3D(position, scale)
    , Box(position, scale)
    , VulkanBox(renderer, position, scale)
    , VulkanTexturedDrawable(texture, specularTexture)
{
}
} // namespace Spark::Render
