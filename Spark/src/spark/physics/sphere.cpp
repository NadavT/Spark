#include "sphere.h"

#include "spark/core/log.h"

#include "bounding/sphere_bounding.h"

namespace Spark::Physics
{
SPARK_API Sphere::Sphere(glm::vec3 position, float radius)
    : Object3D(position)
    , m_radius(radius)
    , m_bounding(position, radius)
{
}

SPARK_API void Sphere::move(glm::vec3 position)
{
    Object3D::move(position);
    m_bounding.setPosition(m_position);
}

SPARK_API void Sphere::scale(glm::vec3 scale)
{
    SPARK_CORE_ASSERT(scale.x == scale.y && scale.x == scale.z, "Sphere should remain symetrical");
    m_radius *= scale.x;
    m_bounding.setRadius(m_radius);
}

SPARK_API void Sphere::setPosition(glm::vec3 position)
{
    Object3D::setPosition(position);
    m_bounding.setPosition(m_position);
}

SPARK_API void Sphere::setScale(glm::vec3 scale)
{
    SPARK_CORE_ASSERT(scale.x == scale.y && scale.x == scale.z, "Sphere should remain symetrical");
    m_radius = scale.x;
    m_bounding.setRadius(m_radius);
}

const Object3DBounding &Sphere::getBoundingObject() const
{
    return m_bounding;
}
} // namespace Spark::Physics