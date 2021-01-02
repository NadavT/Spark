#pragma once
#include "platform/vulkan/drawables/box.h"
#include "platform/vulkan/drawables/textured_drawable.h"
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/vertex/vertex3d.h"

namespace Spark::Render
{
class VulkanTexturedBox : public virtual VulkanBox, public virtual VulkanTexturedDrawable
{
  public:
    VulkanTexturedBox(VulkanRenderer &renderer, glm::vec3 position, const VulkanTexture &texture,
                      const VulkanTexture &specularTexture, glm::vec3 scale = glm::vec3(1.0));
};
} // namespace Spark::Render
