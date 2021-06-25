#include "cylinder.h"

#include "spark/core/log.h"

#include "bounding/box_bounding.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Spark::Physics
{
Cylinder::Cylinder(glm::vec3 position, float baseRadius, float topRadius, float height)
    : Object3D(position)
    , m_scaleMatrix(glm::scale(glm::mat4(1), glm::vec3((baseRadius > topRadius) ? baseRadius : topRadius,
                                                       (baseRadius > topRadius) ? baseRadius : topRadius, height)))
    , m_bounding(glm::translate(glm::mat4(1), m_position), m_scaleMatrix, m_rotationMatrix)
{
}

void Cylinder::move(glm::vec3 position)
{
    Object3D::move(position);
    m_bounding.setTranslation(glm::translate(glm::mat4(1), m_position));
}

void Cylinder::scale(glm::vec3 scale)
{
    m_scaleMatrix = glm::scale(m_scaleMatrix, scale);
    m_bounding.setScale(m_scaleMatrix);
}

void Cylinder::rotate(float angle, glm::vec3 axis)
{
    Object3D::rotate(angle, axis);
    m_bounding.setRotation(m_rotationMatrix);
}

void Cylinder::setPosition(glm::vec3 position)
{
    Object3D::setPosition(position);
    m_bounding.setTranslation(glm::translate(glm::mat4(1), m_position));
}

void Cylinder::setScale(glm::vec3 scale)
{
    m_scaleMatrix = glm::scale(glm::mat4(1), scale);
    m_bounding.setScale(m_scaleMatrix);
}

void Cylinder::setRotation(float angle, glm::vec3 axis)
{
    Object3D::setRotation(angle, axis);
    m_bounding.setRotation(m_rotationMatrix);
}

const Object3DBounding &Cylinder::getBoundingObject() const
{
    return m_bounding;
}

ObjectType Cylinder::getObjectType() const
{
    return ObjectType::Simple;
}
} // namespace Spark::Physics