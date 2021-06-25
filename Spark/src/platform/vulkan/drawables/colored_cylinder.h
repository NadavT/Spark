#pragma once
#include "platform/vulkan/drawables/colored_drawable.h"
#include "platform/vulkan/drawables/cylinder.h"

namespace Spark::Render
{
class VulkanColoredCylinder : public virtual VulkanCylinder, public virtual VulkanColoredDrawable
{
  public:
    VulkanColoredCylinder(VulkanRenderer &renderer, glm::vec3 position, float baseRadius, float topRadius, float height,
                          int sectors, int stacks, glm::vec3 color, glm::vec3 scale = glm::vec3(1.0));
};
} // namespace Spark::Render
