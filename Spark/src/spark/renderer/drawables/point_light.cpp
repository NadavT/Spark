#include "point_light.h"

#ifdef SPARK_PLATFORM_VULKAN
    #include "platform/vulkan/drawables/point_light.h"
    #include "platform/vulkan/renderer.h"
#endif // SPARK_PLATFORM_VULKAN

namespace Spark::Render
{

PointLight::PointLight(glm::vec3 position, glm::vec3 color, std::unique_ptr<Object3D> object, bool isLit)
    : Object3D()
    , m_position(position)
    , m_color(color)
    , m_isLit(isLit)
    , m_object(std::move(object))
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

void PointLight::move(glm::vec3 position, bool moveObject)
{
    m_position += position;
    if (moveObject)
    {
        m_object->move(position);
    }
}

void PointLight::setPosition(glm::vec3 position, bool moveObject)
{
    m_position = position;
    if (moveObject)
    {
        m_object->setPosition(position);
    }
}

glm::vec3 PointLight::getPosition()
{
    return m_position;
}

SPARK_API Object3D &PointLight::getObject()
{
    return *m_object;
}

SPARK_API Physics::Object3D &PointLight::getPhysicsObject() const
{
    return m_object->getPhysicsObject();
}

SPARK_API std::shared_ptr<Drawable3D> PointLight::getDrawable()
{
    return m_object->getDrawable();
}

std::unique_ptr<PointLight> createPointLight(glm::vec3 position, glm::vec3 color, std::unique_ptr<Object3D> object,
                                             bool isLit)
{
#ifdef SPARK_PLATFORM_VULKAN
    return std::make_unique<VulkanPointLight>(position, color, std::move(object), isLit);
#endif // SPARK_PLATFORM_VULKAN
}

} // namespace Spark::Render