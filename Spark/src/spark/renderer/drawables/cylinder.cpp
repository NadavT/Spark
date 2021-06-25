#include "cylinder.h"

#include "spark/core/application.h"

#ifdef SPARK_PLATFORM_VULKAN
    #include "platform/vulkan/drawables/colored_cylinder.h"
    #include "platform/vulkan/drawables/textured_cylinder.h"
    #include "platform/vulkan/renderer.h"
    #include "platform/vulkan/resource/texture.h"

#endif // SPARK_PLATFORM_VULKAN

namespace Spark::Render
{
Cylinder::Cylinder(glm::vec3 position, float baseRadius, float topRadius, float height, int sectors, int stacks,
                   glm::vec3 scale)
    : Drawable3D(position, scale)
    , m_baseRadius(baseRadius)
    , m_topRadius(topRadius)
    , m_height(height)
    , m_sectors(sectors)
    , m_stacks(stacks)
{
}

std::unique_ptr<Drawable3D> createCylinder(glm::vec3 position, const Texture &texture, const Texture &specularTexture,
                                           float baseRadius, float topRadius, float height, int sectors, int stacks,
                                           glm::vec3 scale)
{
#ifdef SPARK_PLATFORM_VULKAN
    VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
    const VulkanTexture &vulkanTexture = reinterpret_cast<const VulkanTexture &>(texture);
    const VulkanTexture &vulkanSpecularTexture = reinterpret_cast<const VulkanTexture &>(specularTexture);
    return std::make_unique<VulkanTexturedCylinder>(renderer, position, baseRadius, topRadius, height, sectors, stacks,
                                                    vulkanTexture, vulkanSpecularTexture, scale);
#endif // SPARK_PLATFORM_VULKAN
}

std::unique_ptr<Drawable3D> createCylinder(glm::vec3 position, glm::vec3 color, float baseRadius, float topRadius,
                                           float height, int sectors, int stacks, glm::vec3 scale)
{
#ifdef SPARK_PLATFORM_VULKAN
    VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
    return std::make_unique<VulkanColoredCylinder>(renderer, position, baseRadius, topRadius, height, sectors, stacks,
                                                   color, scale);
#endif // SPARK_PLATFORM_VULKAN
}
} // namespace Spark::Render
