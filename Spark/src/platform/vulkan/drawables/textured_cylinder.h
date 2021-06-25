#pragma once
#include "platform/vulkan/drawables/cylinder.h"
#include "platform/vulkan/drawables/textured_drawable.h"

namespace Spark::Render
{
class VulkanTexturedCylinder : public virtual VulkanCylinder, public virtual VulkanTexturedDrawable
{
  public:
    VulkanTexturedCylinder(VulkanRenderer &renderer, glm::vec3 position, float baseRadius, float topRadius,
                           float height, int sectors, int stacks, const VulkanTexture &texture,
                           const VulkanTexture &specularTexture, glm::vec3 scale = glm::vec3(1.0));
};
} // namespace Spark::Render
