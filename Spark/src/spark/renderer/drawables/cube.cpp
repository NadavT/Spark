#include "cube.h"

#include "spark/core/application.h"

#ifdef SPARK_PLATFORM_VULKAN
    #include "platform/vulkan/drawables/colored_cube.h"
    #include "platform/vulkan/drawables/textured_cube.h"
    #include "platform/vulkan/renderer.h"
#endif // SPARK_PLATFORM_VULKAN

namespace Spark::Render
{
Cube::Cube(glm::vec3 position, glm::vec3 scale)
    : Drawable3D(position, scale)
{
}

std::shared_ptr<Drawable3D> createCube(glm::vec3 position, const Texture &texture, const Texture &specularTexture,
                                       glm::vec3 scale)
{
#ifdef SPARK_PLATFORM_VULKAN
    VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
    const VulkanTexture &vulkanTexture = reinterpret_cast<const VulkanTexture &>(texture);
    const VulkanTexture &vulkanSpecularTexture = reinterpret_cast<const VulkanTexture &>(specularTexture);
    return std::make_shared<VulkanTexturedCube>(renderer, position, vulkanTexture, vulkanSpecularTexture, scale);
#endif // SPARK_PLATFORM_VULKAN
}

std::shared_ptr<Drawable3D> createCube(glm::vec3 position, glm::vec3 color, glm::vec3 scale)
{
#ifdef SPARK_PLATFORM_VULKAN
    VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
    return std::make_shared<VulkanColoredCube>(renderer, position, color, scale);
#endif // SPARK_PLATFORM_VULKAN
}
} // namespace Spark::Render
