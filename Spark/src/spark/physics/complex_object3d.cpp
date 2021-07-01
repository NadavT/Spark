#include "complex_object3d.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Spark::Physics
{
ComplexObject3D::ComplexObject3D(const std::vector<std::unique_ptr<Mesh>> &meshes, glm::vec3 position, glm::vec3 scale)
    : Object3D(position)
    , m_meshes(meshes)
    , m_bounding(glm::translate(glm::mat4(1), m_position), glm::scale(glm::mat4(1), scale), m_rotationMatrix)
{
}

ComplexObject3D::ComplexObject3D(const std::vector<std::unique_ptr<Mesh>> &meshes, const BoxBounding &boxBound)
    : Object3D(boxBound.getTranslation() * glm::vec4(1))
    , m_meshes(meshes)
    , m_bounding(boxBound)
{
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
