#include "textured_cube.h"

#include "platform/vulkan/renderer.h"

#include "spark/core/application.h"

namespace Spark
{
VulkanTexturedCube::VulkanTexturedCube(VulkanRenderer &renderer, glm::vec3 position, const VulkanTexture &texture,
                                       const VulkanTexture &specularTexture, glm::vec3 scale)
    : VulkanCube(renderer, position, scale)
    , VulkanTexturedDrawable(texture, specularTexture)
    , Cube(position, scale)
{
}
} // namespace Spark
