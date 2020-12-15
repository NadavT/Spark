#include "cube.h"

#include "spark/core/application.h"

#ifdef SPARK_PLATFORM_VULKAN
    #include "platform/vulkan/drawables/colored_cube.h"
    #include "platform/vulkan/drawables/textured_cube.h"
    #include "platform/vulkan/renderer.h"
#endif // SPARK_PLATFORM_VULKAN

namespace Spark
{
Cube::Cube(CubeType type, glm::vec3 position, glm::vec3 scale)
    : m_type(type)
    , m_position(position)
    , m_scale(scale)
    , m_highlight(false)
    , m_highlightColor{1.0f, 1.0f, 1.0f}
    , m_highlightWidth(0.1f)
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

SPARK_API bool Cube::isHighlighted() const
{
    return m_highlight;
}

SPARK_API glm::vec3 Cube::getHighlightColor() const
{
    return m_highlightColor;
}

SPARK_API float Cube::getHighlightWidth() const
{
    return m_highlightWidth;
}

SPARK_API void Cube::highlight()
{
    m_highlight = true;
}

SPARK_API void Cube::unhighlight()
{
    m_highlight = false;
}

SPARK_API void Cube::setHighlightColor(glm::vec3 color)
{
    m_highlightColor = color;
}

SPARK_API void Cube::setHighlightWidth(float width)
{
    m_highlightWidth = width;
}

CubeType Cube::getType() const
{
    return m_type;
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
