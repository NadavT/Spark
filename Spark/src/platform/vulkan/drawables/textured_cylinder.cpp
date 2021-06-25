#include "textured_cylinder.h"

#include "platform/vulkan/renderer.h"

#include "spark/core/application.h"

namespace Spark::Render
{
VulkanTexturedCylinder::VulkanTexturedCylinder(VulkanRenderer &renderer, glm::vec3 position, float baseRadius,
                                               float topRadius, float height, int sectors, int stacks,
                                               const VulkanTexture &texture, const VulkanTexture &specularTexture,
                                               glm::vec3 scale)
    : VulkanCylinder(renderer, position, baseRadius, topRadius, height, sectors, stacks, scale)
    , VulkanTexturedDrawable(texture, specularTexture)
    , Cylinder(position, baseRadius, topRadius, height, sectors, stacks, scale)
    , Drawable3D(position, scale)
{
}
} // namespace Spark::Render
