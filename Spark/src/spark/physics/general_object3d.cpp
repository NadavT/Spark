#include "general_object3d.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Spark::Physics
{
GeneralObject3D::GeneralObject3D(glm::vec3 position, glm::vec3 scale)
    : Object3D(position)
    , m_scaleMatrix(glm::scale(glm::mat4(1), scale))
    , m_bounding(glm::translate(glm::mat4(1), m_position), m_scaleMatrix, m_rotationMatrix)
{
}

GeneralObject3D::GeneralObject3D(const BoxBounding &boxBound)
    : Object3D(boxBound.getTranslation() * glm::vec4(1))
    , m_scaleMatrix(boxBound.getScale())
    , m_bounding(boxBound)
{
}

void GeneralObject3D::move(glm::vec3 position)
{
    Object3D::move(position);
    m_bounding.setTranslation(glm::translate(glm::mat4(1), m_position));
}

void GeneralObject3D::scale(glm::vec3 scale)
{
    m_scaleMatrix = glm::scale(m_scaleMatrix, scale);
    m_bounding.setScale(m_scaleMatrix);
}

void GeneralObject3D::rotate(float angle, glm::vec3 axis)
{
    Object3D::rotate(angle, axis);
    m_bounding.setRotation(m_rotationMatrix);
}

void GeneralObject3D::setPosition(glm::vec3 position)
{
    Object3D::setPosition(position);
    m_bounding.setTranslation(glm::translate(glm::mat4(1), m_position));
}

void GeneralObject3D::setScale(glm::vec3 scale)
{
    m_scaleMatrix = glm::scale(glm::mat4(1), scale);
    m_bounding.setScale(m_scaleMatrix);
}

void GeneralObject3D::setRotation(float angle, glm::vec3 axis)
{
    Object3D::setRotation(angle, axis);
    m_bounding.setRotation(m_rotationMatrix);
}

const Object3DBounding &GeneralObject3D::getBoundingObject() const
{
    return m_bounding;
}

} // namespace Spark::Physics
