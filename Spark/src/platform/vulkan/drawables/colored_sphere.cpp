#include "colored_sphere.h"

#include "platform/vulkan/renderer.h"

#include "spark/core/application.h"

namespace Spark
{
VulkanColoredSphere::VulkanColoredSphere(VulkanRenderer &renderer, glm::vec3 position, int sectors, int stacks,
                                         glm::vec3 color, glm::vec3 scale)
    : VulkanSphere(renderer, position, sectors, stacks, scale)
    , VulkanColoredDrawable(color)
    , Sphere(position, sectors, stacks, scale)
    , Drawable3D(position, scale)
{
}
} // namespace Spark
