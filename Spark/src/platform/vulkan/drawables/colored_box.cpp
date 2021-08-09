#include "colored_box.h"

#include "platform/vulkan/renderer.h"

#include "spark/core/application.h"

namespace Spark::Render
{
VulkanColoredCube::VulkanColoredCube(VulkanRenderer &renderer, glm::vec3 position, glm::vec3 color, glm::vec3 scale)
    : Drawable3D(position, scale)
    , Box(position, scale)
    , VulkanBox(renderer, position, scale)
    , VulkanColoredDrawable(color)
{
}
} // namespace Spark::Render
