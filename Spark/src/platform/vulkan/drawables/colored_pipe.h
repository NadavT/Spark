#ifndef SPARK_VULKAN_COLORED_PIPE_H
#define SPARK_VULKAN_COLORED_PIPE_H
#include "platform/vulkan/drawables/colored_drawable.h"
#include "platform/vulkan/drawables/pipe.h"

namespace Spark::Render
{
class VulkanColoredPipe : public virtual VulkanPipe, public virtual VulkanColoredDrawable
{
  public:
    VulkanColoredPipe(VulkanRenderer &renderer, std::vector<glm::vec3> positions, float radius, bool closed,
                      int sectors, glm::vec3 color, glm::vec3 scale = glm::vec3(1.0));
};
} // namespace Spark::Render

#endif /* SPARK_VULKAN_COLORED_PIPE_H */
