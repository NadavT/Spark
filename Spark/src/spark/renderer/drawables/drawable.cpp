#include "drawable.h"

#include "spark/core/log.h"

namespace Spark::Render
{
Drawable::Drawable()
    : m_highlight(false)
    , m_highlightColor{1.0f, 1.0f, 1.0f}
    , m_highlightWidth(defaultHighlightWidth)
    , m_parent(nullptr)
    , m_childs()
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
    for (auto &child : m_childs)
    {
        child->highlight();
    }
}

void Drawable::unhighlight()
{
    m_highlight = false;
    for (auto &child : m_childs)
    {
        child->unhighlight();
    }
}

void Drawable::setHighlightColor(glm::vec3 color)
{
    m_highlightColor = color;
}

void Drawable::setHighlightWidth(float width)
{
    m_highlightWidth = width;
}

void Drawable::setParent(Drawable *parent)
{
    if (m_parent)
    {
        removeParent();
    }

    m_parent = parent;
    if (parent)
    {
        parent->addChild(this);
    }
}

void Drawable::removeParent()
{
    if (m_parent)
    {
        m_parent->removeChild(this);
        m_parent = nullptr;
    }
}

Drawable *Drawable::getParent() const
{
    return m_parent;
}

const std::vector<Drawable *> &Drawable::getChilds() const
{
    return m_childs;
}

void Drawable::addChild(Drawable *child)
{
    m_childs.push_back(child);
}

void Drawable::removeChild(Drawable *child)
{
    auto found_it = std::find(m_childs.begin(), m_childs.end(), child);

    SPARK_CORE_ASSERT(found_it != m_childs.end(), "Tried to remove a child which isn't a child of this parent");
    if (found_it != m_childs.end())
    {
        m_childs.erase(found_it);
    }
}

void Drawable::clearChilds()
{
    m_childs.clear();
}
} // namespace Spark::Render
