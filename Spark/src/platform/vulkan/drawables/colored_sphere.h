#ifndef SPARK_VULKAN_COLORED_SPHERE_H
#define SPARK_VULKAN_COLORED_SPHERE_H
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

#endif /* SPARK_VULKAN_COLORED_SPHERE_H */
