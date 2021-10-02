#include "cylinder.h"

#include "spark/core/log.h"

#include "bounding/box_bounding.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Spark::Physics
{
Cylinder::Cylinder(glm::vec3 position, float baseRadius, float topRadius, float height)
    : Object3D(position)
    , m_bounding(glm::mat4(1),
                 glm::scale(glm::mat4(1), glm::vec3((baseRadius > topRadius) ? baseRadius : topRadius,
                                                    (baseRadius > topRadius) ? baseRadius : topRadius, height)),
                 glm::mat4(1))
{
}

const Object3DBounding &Cylinder::getBoundingObject() const
{
    return m_bounding;
}
} // namespace Spark::Physics
