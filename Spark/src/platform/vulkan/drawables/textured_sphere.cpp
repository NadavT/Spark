#include "textured_sphere.h"

#include "platform/vulkan/renderer.h"

#include "spark/core/application.h"

namespace Spark
{
VulkanTexturedSphere::VulkanTexturedSphere(VulkanRenderer &renderer, glm::vec3 position, int sectors, int stacks,
                                           const VulkanTexture &texture, const VulkanTexture &specularTexture,
                                           glm::vec3 scale)
    : VulkanSphere(renderer, position, sectors, stacks, scale)
    , VulkanTexturedDrawable(texture, specularTexture)
    , Sphere(position, sectors, stacks, scale)
{
}
} // namespace Spark
