#include "drawable3d.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Spark
{

Drawable3D::Drawable3D(glm::vec3 position, glm::vec3 scale)
    : Drawable()
    , m_position(position)
    , m_scale(scale)
{
}

void Drawable3D::move(glm::vec3 position)
{
    m_position += position;
}

void Drawable3D::scale(glm::vec3 scale)
{
    m_scale *= scale;
}

glm::mat4 Drawable3D::getTransformation()
{
    return glm::translate(glm::scale(glm::mat4(1), m_scale), m_position / m_scale);
}

} // namespace Spark
