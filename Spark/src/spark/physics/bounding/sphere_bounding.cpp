#include "sphere_bounding.h"

namespace Spark::Physics
{
SphereBounding::SphereBounding(glm::vec3 position, float radius)
    : Object3DBounding(Object3DBoundingType::Sphere)
    , m_position(position)
    , m_radius(radius)
{
}
} // namespace Spark::Physics