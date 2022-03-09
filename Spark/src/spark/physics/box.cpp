#include "box.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Spark::Physics
{
Box::Box(glm::vec3 position, float width, float length, float height)
    : Object3D(position)
    , m_bounding(glm::mat4(1), glm::scale(glm::mat4(1), glm::vec3(width, length, height)), glm::mat4(1))
{
}

const Object3DBounding &Box::getBoundingObject() const
{
    return m_bounding;
}
} // namespace Spark::Physics