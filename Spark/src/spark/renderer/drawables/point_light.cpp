#include "point_light.h"

#ifdef SPARK_PLATFORM_VULKAN
    #include "platform/vulkan/drawables/point_light.h"
    #include "platform/vulkan/renderer.h"
#endif // SPARK_PLATFORM_VULKAN

namespace Spark::Render
{

PointLight::PointLight(glm::vec3 position, glm::vec3 color, std::unique_ptr<Object3D> object, bool isLit)
    : Object3D()
    , m_isLit(isLit)
    , m_position(position)
    , m_color(color)
    , m_object(std::move(object))
{
    m_object->getDrawable()->setCalculateLight(!m_isLit);
}

void PointLight::turnOn()
{
    m_isLit = true;
    m_object->getDrawable()->setCalculateLight(!m_isLit);
}

void PointLight::turnOff()
{
    m_isLit = false;
    m_object->getDrawable()->setCalculateLight(!m_isLit);
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

void PointLight::move(glm::vec3 position)
{
    m_position += position;
    m_object->move(position);
}

void PointLight::scale(glm::vec3 scale)
{
    m_object->scale(scale);
}

void PointLight::rotate(float angle, glm::vec3 axis, bool worldRelative)
{
    m_object->rotate(angle, axis, worldRelative);
}

void PointLight::setPosition(glm::vec3 position)
{
    m_position = position;
    m_object->setPosition(position);
}

void PointLight::setScale(glm::vec3 scale)
{
    m_object->setScale(scale);
}

void PointLight::setRotation(float angle, glm::vec3 axis)
{
    m_object->setRotation(angle, axis);
}

glm::vec3 PointLight::getPosition()
{
    return m_position;
}

Object3D &PointLight::getObject()
{
    return *m_object;
}

Physics::Object3D &PointLight::getPhysicsObject() const
{
    return m_object->getPhysicsObject();
}

std::shared_ptr<Drawable3D> &PointLight::getDrawable()
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