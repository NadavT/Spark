#include "box.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Spark::Physics
{
Box::Box(glm::vec3 position, float width, float length, float height)
    : Object3D(position)
    , m_scaleMatrix(glm::scale(glm::mat4(1), glm::vec3(width, length, height)))
    , m_bounding(glm::translate(glm::mat4(1), m_position), m_scaleMatrix, m_rotationMatrix)
{
}

void Box::move(glm::vec3 position)
{
    Object3D::move(position);
    m_bounding.setTranslation(glm::translate(glm::mat4(1), m_position));
}

void Box::scale(glm::vec3 scale)
{
    m_scaleMatrix = glm::scale(m_scaleMatrix, scale);
    m_bounding.setScale(m_scaleMatrix);
}

void Box::rotate(float angle, glm::vec3 axis)
{
    Object3D::rotate(angle, axis);
    m_bounding.setRotation(m_rotationMatrix);
}

void Box::setPosition(glm::vec3 position)
{
    Object3D::setPosition(position);
    m_bounding.setTranslation(glm::translate(glm::mat4(1), m_position));
}

void Box::setScale(glm::vec3 scale)
{
    m_scaleMatrix = glm::scale(glm::mat4(1), scale);
    m_bounding.setScale(m_scaleMatrix);
}

void Box::setRotation(float angle, glm::vec3 axis)
{
    Object3D::setRotation(angle, axis);
    m_bounding.setRotation(m_rotationMatrix);
}

const Object3DBounding &Box::getBoundingObject() const
{
    return m_bounding;
}

ObjectType Box::getObjectType() const
{
    return ObjectType::Simple;
}

} // namespace Spark::Physics