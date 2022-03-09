#include "colored_drawable.h"
namespace Spark::Render
{
VulkanColoredDrawable::VulkanColoredDrawable(glm::vec3 color)
    : m_color(color)
{
}

VulkanColoredDrawable::VulkanColoredDrawable(const VulkanColoredDrawable &other)
    : m_color(other.m_color)
{
}

VulkanColoredDrawable::VulkanColoredDrawable(VulkanColoredDrawable &&other) noexcept
    : m_color(other.m_color)
{
}

VulkanColoredDrawable &VulkanColoredDrawable::operator=(const VulkanColoredDrawable &other)
{
    m_color = other.m_color;
    return *this;
}

VulkanColoredDrawable &VulkanColoredDrawable::operator=(VulkanColoredDrawable &&other) noexcept
{
    m_color = other.m_color;
    return *this;
}

const glm::vec3 VulkanColoredDrawable::getColor() const
{
    return m_color;
}

void VulkanColoredDrawable::setColor(glm::vec3 color, bool childs)
{
    VulkanDrawable::setColor(color, childs);
    m_color = color;
}
VulkanDrawableType VulkanColoredDrawable::getDrawableType() const
{
    return VulkanDrawableType::Colored;
}
} // namespace Spark::Render