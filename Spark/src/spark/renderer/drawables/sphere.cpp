#include "sphere.h"

#include "spark/core/application.h"

#ifdef SPARK_PLATFORM_VULKAN
    #include "platform/vulkan/drawables/colored_sphere.h"
    #include "platform/vulkan/drawables/textured_sphere.h"
    #include "platform/vulkan/renderer.h"
    #include "platform/vulkan/resource/texture.h"

#endif // SPARK_PLATFORM_VULKAN

namespace Spark
{
Sphere::Sphere(glm::vec3 position, int sectors, int stacks, glm::vec3 scale)
    : m_position(position)
    , m_sectors(sectors)
    , m_stacks(stacks)
    , m_scale(scale)
{
}

void Sphere::move(glm::vec3 position)
{
    m_position += position;
}

SPARK_API void Sphere::scale(glm::vec3 scale)
{
    m_scale *= scale;
}

SPARK_API glm::mat4 Sphere::getTransformation()
{
    return glm::translate(glm::scale(glm::mat4(1), m_scale), m_position / m_scale);
}

std::shared_ptr<Drawable> createSphere(glm::vec3 position, const Texture &texture, const Texture &specularTexture,
                                       int sectors, int stacks, glm::vec3 scale)
{
#ifdef SPARK_PLATFORM_VULKAN
    VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
    const VulkanTexture &vulkanTexture = reinterpret_cast<const VulkanTexture &>(texture);
    const VulkanTexture &vulkanSpecularTexture = reinterpret_cast<const VulkanTexture &>(specularTexture);
    return std::make_shared<VulkanTexturedSphere>(renderer, position, sectors, stacks, vulkanTexture,
                                                  vulkanSpecularTexture, scale);
    return NULL;
#endif // SPARK_PLATFORM_VULKAN
}

std::shared_ptr<Drawable> createSphere(glm::vec3 position, glm::vec3 color, int sectors, int stacks, glm::vec3 scale)
{
#ifdef SPARK_PLATFORM_VULKAN
    VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
    return std::make_shared<VulkanColoredSphere>(renderer, position, sectors, stacks, color, scale);
#endif // SPARK_PLATFORM_VULKAN
}
} // namespace Spark
