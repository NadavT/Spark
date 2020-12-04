#include "quad.h"

#include "spark/core/application.h"

#ifdef SPARK_PLATFORM_VULKAN
    #include "platform/vulkan/drawables/quad.h"
    #include "platform/vulkan/renderer.h"
#endif // SPARK_PLATFORM_VULKAN

namespace Spark
{
Quad::Quad(glm::vec2 position, glm::vec2 scale)
    : m_transformation()
{
    m_transformation = glm::translate(glm::scale(glm::mat3(1.0), scale), position);
}

void Quad::move(glm::vec2 position)
{
    m_transformation = glm::translate(m_transformation, position);
}

SPARK_API void Quad::scale(glm::vec2 scale)
{
    m_transformation = glm::scale(m_transformation, scale);
}

SPARK_API glm::mat3 Quad::getTransformation()
{
    return m_transformation;
}

std::shared_ptr<Drawable> createQuad(glm::vec2 position, const Texture &texture, glm::vec2 scale)
{
#ifdef SPARK_PLATFORM_VULKAN
    VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
    const VulkanTexture &vulkanTexture = reinterpret_cast<const VulkanTexture &>(texture);
    return std::make_unique<VulkanQuad>(renderer, position, vulkanTexture, scale);
#endif // SPARK_PLATFORM_VULKAN
}
} // namespace Spark
