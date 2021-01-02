#pragma once
#include "platform/vulkan/drawables/box.h"
#include "platform/vulkan/drawables/colored_drawable.h"

namespace Spark::Render
{
class VulkanColoredCube : public virtual VulkanBox, public virtual VulkanColoredDrawable
{
  public:
    VulkanColoredCube(VulkanRenderer &renderer, glm::vec3 position, glm::vec3 color, glm::vec3 scale = glm::vec3(1.0));
};
} // namespace Spark::Render
