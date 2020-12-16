#pragma once
#include "platform/vulkan/drawables/cube.h"
#include "platform/vulkan/drawables/textured_drawable.h"
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/vertex/vertex3d.h"

namespace Spark
{
class VulkanTexturedCube : public virtual VulkanCube, public virtual VulkanTexturedDrawable
{
  public:
    VulkanTexturedCube(VulkanRenderer &renderer, glm::vec3 position, const VulkanTexture &texture,
                       const VulkanTexture &specularTexture, glm::vec3 scale = glm::vec3(1.0));
};
} // namespace Spark
