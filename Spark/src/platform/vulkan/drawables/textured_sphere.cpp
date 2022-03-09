#include "textured_sphere.h"

namespace Spark::Render
{
VulkanTexturedSphere::VulkanTexturedSphere(VulkanRenderer &renderer, glm::vec3 position, int sectors, int stacks,
                                           const VulkanTexture &texture, const VulkanTexture &specularTexture,
                                           glm::vec3 scale)
    : Drawable3D(position, scale)
    , Sphere(position, sectors, stacks, scale)
    , VulkanSphere(renderer, position, sectors, stacks, scale)
    , VulkanTexturedDrawable(texture, specularTexture)
{
}
} // namespace Spark::Render
