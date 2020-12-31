#include "box_bounding.h"

namespace Spark::Physics
{
BoxBounding::BoxBounding(glm::vec3 position, float width, float length, float height)
    : Object3DBounding(Object3DBoundingType::Box)
    , m_position(position)
    , m_width(width)
    , m_length(length)
    , m_height(height)
{
}
} // namespace Spark::Physics
