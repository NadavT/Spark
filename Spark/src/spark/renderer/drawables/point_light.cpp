#include "point_light.h"

#ifdef SPARK_PLATFORM_VULKAN
    #include "platform/vulkan/drawables/point_light.h"
    #include "platform/vulkan/renderer.h"
#endif // SPARK_PLATFORM_VULKAN

namespace Spark::Render
{

PointLight::PointLight(glm::vec3 position, glm::vec3 color, std::shared_ptr<Drawable3D> &drawable, bool isLit)
    : m_position(position)
    , m_color(color)
    , drawable(drawable)
    , m_isLit(isLit)
{
}

void PointLight::turnOn()
{
    m_isLit = true;
}

void PointLight::turnOff()
{
    m_isLit = false;
}

bool PointLight::isLit()
{
    return m_isLit;
}

glm::vec3 PointLight::getColor()
{
    return m_color;
}

void PointLight::setColor(glm::vec3 color)
{
    m_color = color;
}

void PointLight::move(glm::vec3 position, bool moveDrawable)
{
    m_position += position;
    if (moveDrawable)
    {
        drawable->move(position);
    }
}

void PointLight::setPosition(glm::vec3 position, bool moveDrawable)
{
    m_position = position;
    if (moveDrawable)
    {
        drawable->setPosition(position);
    }
}

glm::vec3 PointLight::getPosition()
{
    return m_position;
}

std::shared_ptr<PointLight> createPointLight(glm::vec3 position, glm::vec3 color, std::shared_ptr<Drawable3D> &drawable,
                                             bool isLit)
{
#ifdef SPARK_PLATFORM_VULKAN
    return std::make_shared<VulkanPointLight>(position, color, drawable, isLit);
#endif // SPARK_PLATFORM_VULKAN
}

} // namespace Spark::Render