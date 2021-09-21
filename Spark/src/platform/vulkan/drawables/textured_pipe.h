#ifndef SPARK_VULKAN_TEXTURED_PIPE_H
#define SPARK_VULKAN_TEXTURED_PIPE_H
#include "platform/vulkan/drawables/pipe.h"
#include "platform/vulkan/drawables/textured_drawable.h"

namespace Spark::Render
{
class VulkanTexturedPipe : public virtual VulkanPipe, public virtual VulkanTexturedDrawable
{
  public:
    VulkanTexturedPipe(VulkanRenderer &renderer, std::vector<glm::vec3> positions, float radius, int sectors,
                       const VulkanTexture &texture, const VulkanTexture &specularTexture,
                       glm::vec3 scale = glm::vec3(1.0));
};
} // namespace Spark::Render

#endif /* SPARK_VULKAN_TEXTURED_PIPE_H */
