#include "colored_cube.h"

#include "platform/vulkan/renderer.h"

#include "spark/core/application.h"

namespace Spark
{
VulkanColoredCube::VulkanColoredCube(VulkanRenderer &renderer, glm::vec3 position, glm::vec3 color, glm::vec3 scale)
    : VulkanCube(renderer, position, scale)
    , VulkanColoredDrawable(color)
    , Cube(position, scale)
    , Drawable3D(position, scale)
{
}
} // namespace Spark
