#include "complex_object3d.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Spark::Physics
{
ComplexObject3D::ComplexObject3D(const std::vector<std::unique_ptr<Mesh>> &meshes, glm::vec3 position, glm::vec3 scale)
    : Object3D(position)
    , m_meshes(meshes)
    , m_scaleMatrix(glm::scale(glm::mat4(1), scale))
    , m_initialBoundsScaleMatrix(m_scaleMatrix)
    , m_bounding(glm::translate(glm::mat4(1), m_position), m_scaleMatrix, m_rotationMatrix)
{
}

ComplexObject3D::ComplexObject3D(const std::vector<std::unique_ptr<Mesh>> &meshes, const BoxBounding &boxBound)
    : Object3D(boxBound.getTranslation() * glm::vec4(1))
    , m_meshes(meshes)
    , m_scaleMatrix(glm::mat4(1))
    , m_initialBoundsScaleMatrix(boxBound.getScale())
    , m_bounding(boxBound)
{
}

void ComplexObject3D::move(glm::vec3 position)
{
    Object3D::move(position);
    m_bounding.setTranslation(glm::translate(glm::mat4(1), m_position));
}

void ComplexObject3D::scale(glm::vec3 scale)
{
    m_scaleMatrix = glm::scale(m_scaleMatrix, scale);
    m_bounding.setScale(m_scaleMatrix * m_initialBoundsScaleMatrix);
}

void ComplexObject3D::rotate(float angle, glm::vec3 axis)
{
    Object3D::rotate(angle, axis);
    m_bounding.setRotation(m_rotationMatrix);
}

void ComplexObject3D::setPosition(glm::vec3 position)
{
    Object3D::setPosition(position);
    m_bounding.setTranslation(glm::translate(glm::mat4(1), m_position));
}

void ComplexObject3D::setScale(glm::vec3 scale)
{
    m_scaleMatrix = glm::scale(glm::mat4(1), scale);
    m_bounding.setScale(m_scaleMatrix * m_initialBoundsScaleMatrix);
}

void ComplexObject3D::setRotation(float angle, glm::vec3 axis)
{
    Object3D::setRotation(angle, axis);
    m_bounding.setRotation(m_rotationMatrix);
}

glm::mat4 ComplexObject3D::getTranslation() const
{
    return glm::translate(glm::mat4(1), m_position);
}

glm::mat4 ComplexObject3D::getScale() const
{
    return m_scaleMatrix;
}

glm::mat4 ComplexObject3D::getRotation() const
{
    return m_rotationMatrix;
}

const std::vector<std::unique_ptr<Mesh>> &ComplexObject3D::getMeshes() const
{
    return m_meshes;
}

const Object3DBounding &ComplexObject3D::getBoundingObject() const
{
    return m_bounding;
}

ObjectType ComplexObject3D::getObjectType() const
{
    return ObjectType::Complex;
}

} // namespace Spark::Physics
