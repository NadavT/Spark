#include "cube.h"

#include "spark/core/application.h"

#ifdef SPARK_PLATFORM_VULKAN
    #include "platform/vulkan/drawables/colored_cube.h"
    #include "platform/vulkan/drawables/textured_cube.h"
    #include "platform/vulkan/renderer.h"
#endif // SPARK_PLATFORM_VULKAN

namespace Spark
{
Cube::Cube(glm::vec3 position, glm::vec3 scale)
    : m_position(position)
    , m_scale(scale)
{
}

void Cube::move(glm::vec3 position)
{
    m_position += position;
}

SPARK_API void Cube::scale(glm::vec3 scale)
{
    m_scale *= scale;
}

SPARK_API glm::mat4 Cube::getTransformation()
{
    return glm::translate(glm::scale(glm::mat4(1), m_scale), m_position / m_scale);
}

std::shared_ptr<Drawable> createCube(glm::vec3 position, const Texture &texture, const Texture &specularTexture,
                                     glm::vec3 scale)
{
#ifdef SPARK_PLATFORM_VULKAN
    VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
    const VulkanTexture &vulkanTexture = reinterpret_cast<const VulkanTexture &>(texture);
    const VulkanTexture &vulkanSpecularTexture = reinterpret_cast<const VulkanTexture &>(specularTexture);
    return std::make_shared<VulkanTexturedCube>(renderer, position, vulkanTexture, vulkanSpecularTexture, scale);
#endif // SPARK_PLATFORM_VULKAN
}

std::shared_ptr<Drawable> createCube(glm::vec3 position, glm::vec3 color, glm::vec3 scale)
{
#ifdef SPARK_PLATFORM_VULKAN
    VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
    return std::make_shared<VulkanColoredCube>(renderer, position, color, scale);
#endif // SPARK_PLATFORM_VULKAN
}
} // namespace Spark
