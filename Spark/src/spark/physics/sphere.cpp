#include "sphere.h"

#include "spark/core/log.h"

#include "bounding/sphere_bounding.h"

namespace Spark::Physics
{
Sphere::Sphere(glm::vec3 position, float radius)
    : Object3D(position)
    , m_radius(radius)
    , m_bounding(glm::vec3(0, 0, 0), radius)
{
}

void Sphere::scale(glm::vec3 scale)
{
    SPARK_CORE_ASSERT(scale.x == scale.y && scale.x == scale.z, "Sphere should remain symmetrical");
    m_radius *= scale.x;
}

void Sphere::setScale(glm::vec3 scale)
{
    SPARK_CORE_ASSERT(scale.x == scale.y && scale.x == scale.z, "Sphere should remain symmetrical");
    m_radius = scale.x;
}

const Object3DBounding &Sphere::getBoundingObject() const
{
    return m_bounding;
}
} // namespace Spark::Physics
