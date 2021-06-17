#include "sphere.h"

#include "spark/core/log.h"

#include "bounding/sphere_bounding.h"

namespace Spark::Physics
{
Sphere::Sphere(glm::vec3 position, float radius)
    : Object3D(position)
    , m_radius(radius)
    , m_bounding(position, radius)
{
}

void Sphere::move(glm::vec3 position)
{
    Object3D::move(position);
    m_bounding.setPosition(m_position);
}

void Sphere::scale(glm::vec3 scale)
{
    SPARK_CORE_ASSERT(scale.x == scale.y && scale.x == scale.z, "Sphere should remain symetrical");
    m_radius *= scale.x;
    m_bounding.setRadius(m_radius);
}

void Sphere::setPosition(glm::vec3 position)
{
    Object3D::setPosition(position);
    m_bounding.setPosition(m_position);
}

void Sphere::setScale(glm::vec3 scale)
{
    SPARK_CORE_ASSERT(scale.x == scale.y && scale.x == scale.z, "Sphere should remain symetrical");
    m_radius = scale.x;
    m_bounding.setRadius(m_radius);
}

const Object3DBounding &Sphere::getBoundingObject() const
{
    return m_bounding;
}

ObjectType Sphere::getObjectType() const
{
    return ObjectType::Simple;
}
} // namespace Spark::Physics