#pragma once
#include "platform/vulkan/drawables/sphere.h"
#include "platform/vulkan/drawables/textured_drawable.h"

namespace Spark
{
class VulkanTexturedSphere : public virtual VulkanSphere, public virtual VulkanTexturedDrawable
{
  public:
    VulkanTexturedSphere(VulkanRenderer &renderer, glm::vec3 position, int sectors, int stacks,
                         const VulkanTexture &texture, const VulkanTexture &specularTexture,
                         glm::vec3 scale = glm::vec3(1.0));
};
} // namespace Spark
