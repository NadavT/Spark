#include "object3d.h"

#include "spark/core/log.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Spark::Physics
{
Object3D::Object3D(glm::vec3 position)
    : m_position(position)
    , m_rotationMatrix(glm::mat4(1))
    , m_parent(nullptr)
    , m_childs()
{
}

glm::vec3 Object3D::getPosition() const
{
    return m_position;
}

void Object3D::move(glm::vec3 position)
{
    m_position += position;
}

void Object3D::rotate(float angle, glm::vec3 axis)
{
    m_rotationMatrix = glm::rotate(m_rotationMatrix, angle, axis);
}

void Object3D::setPosition(glm::vec3 position)
{
    m_position = position;
}

SPARK_API void Object3D::setRotation(float angle, glm::vec3 axis)
{
    m_rotationMatrix = glm::rotate(m_rotationMatrix, angle, axis);
}
void Object3D::setParent(Object3D *parent)
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

void Object3D::removeParent()
{
    if (m_parent)
    {
        m_parent->removeChild(this);
        m_parent = nullptr;
    }
}

Object3D *Object3D::getParent() const
{
    return m_parent;
}

const std::vector<Object3D *> &Object3D::getChilds() const
{
    return m_childs;
}

void Object3D::addChild(Object3D *child)
{
    m_childs.push_back(child);
}

void Object3D::removeChild(Object3D *child)
{
    auto found_it = std::find(m_childs.begin(), m_childs.end(), child);

    SPARK_CORE_ASSERT(found_it != m_childs.end(), "Tried to remove a child which isn't a child of this parent");
    if (found_it != m_childs.end())
    {
        m_childs.erase(found_it);
    }
}

void Object3D::clearChilds()
{
    m_childs.clear();
}
} // namespace Spark::Physics
