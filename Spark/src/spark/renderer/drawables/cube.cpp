#include "cube.h"

#include "spark/core/application.h"

#ifdef SPARK_PLATFORM_VULKAN
#include "platform/vulkan/drawables/cube.h"
#include "platform/vulkan/renderer.h"

#endif // SPARK_PLATFORM_VULKAN

namespace Spark
{
Cube::Cube(glm::vec3 position, glm::vec3 scale)
    : m_transformation()
{
    m_transformation = glm::translate(glm::scale(glm::mat4(1.0), scale), position);
}

void Cube::move(glm::vec3 position)
{
    m_transformation = glm::translate(m_transformation, position);
}

SPARK_API void Cube::scale(glm::vec3 scale)
{
    m_transformation = glm::scale(m_transformation, scale);
}

SPARK_API glm::mat4 Cube::getTransformation()
{
    return m_transformation;
}

std::shared_ptr<Drawable> createCube(glm::vec3 position, const Texture &texture, glm::vec3 scale)
{
#ifdef SPARK_PLATFORM_VULKAN
    VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
    const VulkanTexture &vulkanTexture = reinterpret_cast<const VulkanTexture &>(texture);
    return std::make_unique<VulkanCube>(renderer, position, vulkanTexture, scale);
#endif // SPARK_PLATFORM_VULKAN
}
} // namespace Spark
