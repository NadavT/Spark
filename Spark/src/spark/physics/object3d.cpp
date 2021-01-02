#include "object3d.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Spark::Physics
{
Object3D::Object3D(glm::vec3 position)
    : m_position(position)
    , m_rotationMatrix(glm::mat4(1))
{
}

glm::vec3 Object3D::getPosition() const
{
    return m_position;
}

void Object3D::move(glm::vec3 position)
{
    m_position += position;
}

void Object3D::rotate(float angle, glm::vec3 axis)
{
    m_rotationMatrix = glm::rotate(m_rotationMatrix, angle, axis);
}

void Object3D::setPosition(glm::vec3 position)
{
    m_position = position;
}

SPARK_API void Object3D::setRotation(float angle, glm::vec3 axis)
{
    m_rotationMatrix = glm::rotate(m_rotationMatrix, angle, axis);
}
} // namespace Spark::Physics
