#include "sphere.h"

#include "spark/core/application.h"

#ifdef SPARK_PLATFORM_VULKAN
    #include "platform/vulkan/drawables/colored_sphere.h"
    #include "platform/vulkan/drawables/textured_sphere.h"
    #include "platform/vulkan/renderer.h"
    #include "platform/vulkan/resource/texture.h"

#endif // SPARK_PLATFORM_VULKAN

namespace Spark::Render
{
Sphere::Sphere(glm::vec3 position, int sectors, int stacks, glm::vec3 scale)
    : Drawable3D(position, scale)
    , m_sectors(sectors)
    , m_stacks(stacks)
{
}

std::unique_ptr<Drawable3D> createSphere(glm::vec3 position, const Texture &texture, const Texture &specularTexture,
                                         int sectors, int stacks, glm::vec3 scale)
{
#ifdef SPARK_PLATFORM_VULKAN
    VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
    const VulkanTexture &vulkanTexture = reinterpret_cast<const VulkanTexture &>(texture);
    const VulkanTexture &vulkanSpecularTexture = reinterpret_cast<const VulkanTexture &>(specularTexture);
    return std::make_unique<VulkanTexturedSphere>(renderer, position, sectors, stacks, vulkanTexture,
                                                  vulkanSpecularTexture, scale);
#endif // SPARK_PLATFORM_VULKAN
}

std::unique_ptr<Drawable3D> createSphere(glm::vec3 position, glm::vec3 color, int sectors, int stacks, glm::vec3 scale)
{
#ifdef SPARK_PLATFORM_VULKAN
    VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
    return std::make_unique<VulkanColoredSphere>(renderer, position, sectors, stacks, color, scale);
#endif // SPARK_PLATFORM_VULKAN
}
} // namespace Spark::Render
