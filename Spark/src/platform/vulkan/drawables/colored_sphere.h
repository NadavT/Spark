#pragma once
#include "platform/vulkan/drawables/colored_drawable.h"
#include "platform/vulkan/drawables/sphere.h"

namespace Spark::Render
{
class VulkanColoredSphere : public virtual VulkanSphere, public virtual VulkanColoredDrawable
{
  public:
    VulkanColoredSphere(VulkanRenderer &renderer, glm::vec3 position, int sectors, int stacks, glm::vec3 color,
                        glm::vec3 scale = glm::vec3(1.0));
};
} // namespace Spark::Render
