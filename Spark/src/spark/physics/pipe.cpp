#include "pipe.h"

#include "spark/core/log.h"
#include "spark/utils/math.h"

#include "bounding/sphere_bounding.h"
#include "ray_casting.h"

namespace Spark::Physics
{
Pipe::Pipe(std::vector<glm::vec3> positions, float radius)
    : Object3D(mid_range(positions))
    , m_positions(positions)
    , m_radius(radius)
    , m_originalRadius(radius)
    , m_bounding(glm::mat4(1), glm::mat4(1), glm::mat4(1))
{
    std::vector<float> x(positions.size(), 0), y(positions.size(), 0), z(positions.size(), 0);
    std::transform(positions.begin(), positions.end(), x.begin(), [](glm::vec3 &v) { return v.x; });
    std::transform(positions.begin(), positions.end(), y.begin(), [](glm::vec3 &v) { return v.y; });
    std::transform(positions.begin(), positions.end(), z.begin(), [](glm::vec3 &v) { return v.z; });
    auto [x_min, x_max] = std::minmax_element(x.begin(), x.end());
    auto [y_min, y_max] = std::minmax_element(y.begin(), y.end());
    auto [z_min, z_max] = std::minmax_element(z.begin(), z.end());
    m_bounding.setScale(glm::scale(glm::mat4(1), glm::vec3(x_max - x_min, y_max - y_min, z_max - z_min)));
}

void Pipe::scale(glm::vec3 scale)
{
    SPARK_CORE_ASSERT(scale.x == scale.y && scale.x == scale.z, "Pipes should be scaled symmetrically");
    m_radius *= scale.x;
    Object3D::scale(scale);
}

void Pipe::setScale(glm::vec3 scale)
{
    SPARK_CORE_ASSERT(scale.x == scale.y && scale.x == scale.z, "Pipes should be scaled symmetrically");
    m_radius = m_originalRadius * scale.x;
    Object3D::scale(scale);
}

const Object3DBounding &Pipe::getBoundingObject() const
{
    return m_bounding;
}

float Pipe::getRayDistanceFromObject(Ray3D ray) const
{
    float distance = -1;
    for (int i = 0; i < m_positions.size() - 2; i++)
    {
        glm::vec3 p0 = getTransformation() * glm::vec4(m_positions[i], 1.0f);
        glm::vec3 p1 = getTransformation() * glm::vec4(m_positions[i + 1], 1.0f);

        // Calc plane
        glm::vec3 n = glm::normalize(glm::cross(glm::cross((p1 - p0), ray.direction), (p1 - p0)));
        glm::vec3 d = glm::normalize(glm::cross(n, (p1 - p0)));
        glm::vec3 v0 = p0 + d * m_radius;
        glm::vec3 v1 = p0 - d * m_radius;
        glm::vec3 v2 = p1 + d * m_radius;
        glm::vec3 v3 = p1 - d * m_radius;
        float intersection = glm::dot((p0 - ray.source), n) / glm::dot(ray.direction, n);
        if (intersection < 0)
        {
        }
        glm::vec3 intersectionPoint = ray.source + intersection * ray.direction;
        float curDistance = glm::distance(intersectionPoint, ray.source);
        if (isPointInTriangle(intersectionPoint, v0, v1, v2) || isPointInTriangle(intersectionPoint, v1, v2, v3))
        {
            if (distance == -1 || curDistance < distance)
            {
                distance = curDistance;
            }
        }
    }

    return distance;
}

} // namespace Spark::Physics
