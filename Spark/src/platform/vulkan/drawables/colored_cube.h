#pragma once
#include "platform/vulkan/drawables/colored_drawable.h"
#include "platform/vulkan/drawables/cube.h"

namespace Spark
{
class VulkanColoredCube : public virtual VulkanCube, public virtual VulkanColoredDrawable
{
  public:
    VulkanColoredCube(VulkanRenderer &renderer, glm::vec3 position, glm::vec3 color, glm::vec3 scale = glm::vec3(1.0));
};
} // namespace Spark
