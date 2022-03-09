#include "colored_sphere.h"

namespace Spark::Render
{
VulkanColoredSphere::VulkanColoredSphere(VulkanRenderer &renderer, glm::vec3 position, int sectors, int stacks,
                                         glm::vec3 color, glm::vec3 scale)
    : Drawable3D(position, scale)
    , Sphere(position, sectors, stacks, scale)
    , VulkanSphere(renderer, position, sectors, stacks, scale)
    , VulkanColoredDrawable(color)
{
}
} // namespace Spark::Render
