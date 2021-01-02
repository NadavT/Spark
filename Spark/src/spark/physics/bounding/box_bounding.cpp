#include "box_bounding.h"

namespace Spark::Physics
{
BoxBounding::BoxBounding(glm::mat4 translation, glm::mat4 scale, glm::mat4 rotation)
    : Object3DBounding(Object3DBoundingType::Box)
    , m_translationMatrix(translation)
    , m_scaleMatrix(scale)
    , m_rotationMatrix(rotation)
{
}
} // namespace Spark::Physics
