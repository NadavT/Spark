#include "point_light.h"

#ifdef SPARK_PLATFORM_VULKAN
    #include "platform/vulkan/drawables/point_light.h"
    #include "platform/vulkan/renderer.h"
#endif // SPARK_PLATFORM_VULKAN

namespace Spark
{
SPARK_API std::shared_ptr<PointLight> createPointLight(glm::vec3 position, glm::vec3 color,
                                                       std::shared_ptr<Drawable> &drawable, bool isLit)
{
#ifdef SPARK_PLATFORM_VULKAN
    return std::make_shared<VulkanPointLight>(position, color, drawable, isLit);
#endif // SPARK_PLATFORM_VULKAN
}
} // namespace Spark