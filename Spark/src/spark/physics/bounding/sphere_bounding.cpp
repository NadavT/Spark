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

float SphereBounding::getRayDistanceFromObject(Ray3D ray, glm::mat4 transformation) const
{
    const float a = 1.0f;
    glm::vec3 position = glm::vec3(transformation * getTransformation() * glm::vec4(0, 0, 0, 1));
    float b = 2 * glm::dot(ray.direction, ray.source - position);
    float c = static_cast<float>(glm::pow(glm::length(ray.source - position), 2) - glm::pow(getRadius(), 2));
    float res = static_cast<float>(glm::pow(b, 2) - (4 * a * c));
    if (res < 0)
    {
        return -1;
    }

    float sol1 = (-b + glm::sqrt(res)) / (2 * a);
    float sol2 = (-b - glm::sqrt(res)) / (2 * a);

    if (sol1 < 0 && sol2 < 0)
    {
        return -1;
    }
    else if (sol1 < 0)
    {
        return glm::length(sol2 * ray.direction);
    }
    else
    {
        float distance1 = glm::length(sol1 * ray.direction);
        float distance2 = glm::length(sol2 * ray.direction);
        return (distance1 < distance2) ? distance1 : distance2;
    }
}
} // namespace Spark::Physics