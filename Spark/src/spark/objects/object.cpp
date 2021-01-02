#include "object.h"
namespace Spark
{
SPARK_API Object3D::Object3D(std::shared_ptr<Render::Drawable3D> &drawable,
                             std::unique_ptr<Physics::Object3D> physicObject)
    : m_drawable(drawable)
    , m_physicsObject(std::move(physicObject))
{
}

SPARK_API void Object3D::move(glm::vec3 position)
{
    m_drawable->move(position);
    m_physicsObject->move(position);
}

SPARK_API void Object3D::scale(glm::vec3 scale)
{
    m_drawable->scale(scale);
    m_physicsObject->scale(scale);
}

SPARK_API void Object3D::rotate(float angle, glm::vec3 axis)
{
    m_drawable->rotate(angle, axis);
    m_physicsObject->rotate(angle, axis);
}

SPARK_API void Object3D::setPosition(glm::vec3 position)
{
    m_drawable->setPosition(position);
    m_physicsObject->setPosition(position);
}

SPARK_API void Object3D::setScale(glm::vec3 scale)
{
    m_drawable->setScale(scale);
    m_physicsObject->setScale(scale);
}

SPARK_API void Object3D::setRotation(float angle, glm::vec3 axis)
{
    m_drawable->setRotation(angle, axis);
    m_physicsObject->setRotation(angle, axis);
}
} // namespace Spark
