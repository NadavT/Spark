#include "sphere_bounding.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Spark::Physics
{
SphereBounding::SphereBounding(glm::vec3 position, float radius)
    : Object3DBounding(Object3DBoundingType::Sphere)
    , m_position(position)
    , m_radius(radius)
{
}

glm::mat4 SphereBounding::getTransformation() const
{
    return glm::translate(glm::mat4(1), m_position) * glm::scale(glm::mat4(1), glm::vec3(m_radius, m_radius, m_radius));
}
} // namespace Spark::Physics