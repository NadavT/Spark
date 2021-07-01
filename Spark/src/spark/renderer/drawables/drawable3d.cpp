#include "drawable3d.h"

#include <glm/gtc/matrix_transform.hpp>

#ifdef SPARK_PLATFORM_VULKAN
    #include "platform/vulkan/drawables/model.h"
    #include "platform/vulkan/renderer.h"
#endif // SPARK_PLATFORM_VULKAN

namespace Spark::Render
{
Drawable3D::Drawable3D(glm::vec3 position, glm::vec3 scale, float rotationAngle, glm::vec3 rotationAxis)
    : Drawable()
    , m_translationMatrix(glm::translate(glm::mat4(1), position))
    , m_scaleMatrix(glm::scale(glm::mat4(1), scale))
    , m_rotationMatrix(glm::rotate(glm::mat4(1), rotationAngle, rotationAxis))
{
    setHighlightWidth(getHighlightWidth() / ((scale.x + scale.y + scale.z) / 3));
}

std::unique_ptr<Drawable3D> createModelDrawable(const Model &model, glm::vec3 position, glm::vec3 scale)
{
    return std::make_unique<VulkanDrawableModel>(model, position, scale);
}

void Drawable3D::move(glm::vec3 position)
{
    m_translationMatrix = glm::translate(m_translationMatrix, position);
}

void Drawable3D::scale(glm::vec3 scale)
{
    m_scaleMatrix = glm::scale(m_scaleMatrix, scale);
    setHighlightWidth(getHighlightWidth() / ((scale.x + scale.y + scale.z) / 3));
}

SPARK_API void Drawable3D::rotate(float angle, glm::vec3 axis)
{
    m_rotationMatrix = glm::rotate(m_rotationMatrix, angle, axis);
}

SPARK_API void Drawable3D::setPosition(glm::vec3 position)
{
    m_translationMatrix = glm::translate(glm::mat4(1), position);
}

SPARK_API void Drawable3D::setScale(glm::vec3 scale)
{
    m_scaleMatrix = glm::scale(glm::mat4(1), scale);
    setHighlightWidth(defaultHighlightWidth / ((scale.x + scale.y + scale.z) / 3));
}

SPARK_API void Drawable3D::setRotation(float angle, glm::vec3 axis)
{
    m_rotationMatrix = glm::rotate(glm::mat4(1), angle, axis);
}

glm::mat4 Drawable3D::getTransformation() const
{
    if (m_parent)
    {
        return m_parent->getTransformation() * m_translationMatrix * m_rotationMatrix * m_scaleMatrix;
    }
    else
    {
        return m_translationMatrix * m_rotationMatrix * m_scaleMatrix;
    }
}
} // namespace Spark::Render
