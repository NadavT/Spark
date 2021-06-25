#include "colored_cylinder.h"

#include "platform/vulkan/renderer.h"

#include "spark/core/application.h"

namespace Spark::Render
{
VulkanColoredCylinder::VulkanColoredCylinder(VulkanRenderer &renderer, glm::vec3 position, float baseRadius,
                                             float topRadius, float height, int sectors, int stacks, glm::vec3 color,
                                             glm::vec3 scale)
    : VulkanCylinder(renderer, position, baseRadius, topRadius, height, sectors, stacks, scale)
    , VulkanColoredDrawable(color)
    , Cylinder(position, baseRadius, topRadius, height, sectors, stacks, scale)
    , Drawable3D(position, scale)
{
}
} // namespace Spark::Render
