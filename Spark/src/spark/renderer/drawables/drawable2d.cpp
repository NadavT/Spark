#include "drawable2d.h"

#include <glm/gtx/matrix_transform_2d.hpp>

namespace Spark
{

Drawable2D::Drawable2D(glm::vec2 position, glm::vec2 scale)
    : Drawable()
    , m_position(position)
    , m_scale(scale)
{
}

void Drawable2D::move(glm::vec2 position)
{
    m_position += position;
}

void Drawable2D::scale(glm::vec2 scale)
{
    m_scale *= scale;
}

glm::mat4 Drawable2D::getTransformation()
{
    return glm::mat4(glm::translate(glm::scale(glm::mat3(1), m_scale), m_position / m_scale));
}

} // namespace Spark
