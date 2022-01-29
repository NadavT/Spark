#include "math.h"
namespace Spark
{
glm::vec3 getIntersectionNormalRay(glm::vec3 position, glm::vec3 normal, Physics::Ray3D ray)
{
    float intersection = glm::dot((position - ray.source), normal) / glm::dot(ray.direction, normal);
    return ray.source + intersection * ray.direction;
}
} // namespace Spark
