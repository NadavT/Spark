#include "object3d.h"

namespace Spark::Physics
{
SPARK_API Object3D::Object3D(glm::vec3 position)
    : m_position(position)
{
}

SPARK_API void Object3D::move(glm::vec3 position)
{
    m_position += position;
}

SPARK_API void Object3D::setPosition(glm::vec3 position)
{
    m_position = position;
}
} // namespace Spark::Physics
