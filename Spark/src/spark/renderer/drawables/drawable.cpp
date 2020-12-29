#include "drawable.h"

namespace Spark::Render
{
Drawable::Drawable()
    : m_highlight(false)
    , m_highlightColor{1.0f, 1.0f, 1.0f}
    , m_highlightWidth(0.1f)
{
}

bool Drawable::isHighlighted() const
{
    return m_highlight;
}

glm::vec3 Drawable::getHighlightColor() const
{
    return m_highlightColor;
}

float Drawable::getHighlightWidth() const
{
    return m_highlightWidth;
}

void Drawable::highlight()
{
    m_highlight = true;
}

void Drawable::unhighlight()
{
    m_highlight = false;
}

void Drawable::setHighlightColor(glm::vec3 color)
{
    m_highlightColor = color;
}

void Drawable::setHighlightWidth(float width)
{
    m_highlightWidth = width;
}
} // namespace Spark::Render
